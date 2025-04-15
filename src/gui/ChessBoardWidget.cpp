#include "ChessBoardWidget.h"
#include "model/pieces/Piece.h"
#include "model/Move.h"
#include "core/Utils.h"
#include "model/ChessModel.h"
#include "Constants.h"
#include "DrawingUtils.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QDataStream>
#include <QIODevice>
#include <QDebug>
#include <QPen>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QFontMetrics>
#include <vector>
#include <algorithm>

ChessBoardWidget::ChessBoardWidget(ChessModel* model, QWidget *parent)
    : QWidget(parent), chessModel(model), selectedSquare{-1, -1}, dragIndicatorSource{-1, -1}
{
    setAcceptDrops(true);
}

ChessBoardWidget::~ChessBoardWidget() = default;

QSize ChessBoardWidget::minimumSizeHint() const { return QSize(320, 320); }
QSize ChessBoardWidget::sizeHint() const { return QSize(480, 480); }

int ChessBoardWidget::squareSize() const {
    int w = width();
    int h = height();
    return qMin(w, h) / 8;
}

QRect ChessBoardWidget::squareRect(const Position& pos) const {
    int sSize = squareSize();
    if (sSize == 0 || !pos.isValid()) return QRect();
    return QRect(pos.col * sSize, (7 - pos.row) * sSize, sSize, sSize);
}

Position ChessBoardWidget::positionFromPoint(const QPoint& point) const {
    int sSize = squareSize();
    if (sSize == 0 || !rect().contains(point)) return {-1, -1};

    int col = point.x() / sSize;
    int row = 7 - (point.y() / sSize);
    if (col < 0 || col > 7 || row < 0 || row > 7) return {-1, -1};

    return {row, col};
}

void ChessBoardWidget::resetInteractionState(bool doUpdate) {
    selectedSquare = {-1, -1};
    draggedPiece = nullptr;
    dragIndicatorSource = {-1, -1};
    if (doUpdate) update();
}

void ChessBoardWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int sSize = squareSize();
    if (sSize == 0 || !chessModel) return;

    // Draw chessboard squares
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            painter.setBrush((row + col) % 2 ? ChessConstants::DARK_SQUARE_COLOR : ChessConstants::LIGHT_SQUARE_COLOR);
            painter.drawRect(squareRect({row, col}));
        }
    }

    // Highlight selected square
    if (selectedSquare.isValid()) {
        painter.setBrush(ChessConstants::SELECTION_HIGHLIGHT_COLOR);
        painter.drawRect(squareRect(selectedSquare));
    }

    // Draw pieces
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Position currentPos{row, col};
            
            if (dragIndicatorSource.isValid() && currentPos == dragIndicatorSource)
                continue;
    
            if (Piece* piece = chessModel->getPiece(row, col)) {
                ChessDrawingUtils::drawPiece(painter, piece, squareRect(currentPos), sSize);
            }
        }
    }
    

    // Show valid move indicators
    Position indicatorPos = dragIndicatorSource.isValid() ? dragIndicatorSource : selectedSquare;
    if (indicatorPos.isValid()) {
        std::vector<Position> validMoves = chessModel->getValidMoves(indicatorPos);
        Piece* sourcePiece = chessModel->getPiece(indicatorPos.row, indicatorPos.col);
        if (sourcePiece && sourcePiece->isWhite == chessModel->isWhiteToMove()) { 
            painter.setPen(Qt::NoPen);
            painter.setBrush(ChessConstants::MOVE_INDICATOR_COLOR);
            int radius = sSize / 7;
            for (const auto& move : chessModel->getValidMoves(indicatorPos)) {
                painter.drawEllipse(squareRect(move).center(), radius, radius);
            }
        }
    }

    // Draw coordinates
    painter.setFont(QFont("Arial", qMax(8, sSize/7), QFont::Bold));
    QFontMetrics fm(painter.font());
    int padding = sSize / 15;

    // File letters (a-h)
    for (int col = 0; col < 8; ++col) {
        bool dark = (0 + col) % 2;
        painter.setPen(dark ? ChessConstants::LIGHT_SQUARE_COLOR : ChessConstants::DARK_SQUARE_COLOR);
        QString file = QString(QChar::fromLatin1('a' + col));
        QRect square = squareRect({0, col});
        painter.drawText(square.right() - fm.horizontalAdvance(file) - padding, 
                        square.bottom() - fm.descent() - padding, file);
    }

    // Rank numbers (1-8)
    for (int row = 0; row < 8; ++row) {
        bool dark = (row + 0) % 2;
        painter.setPen(dark ? ChessConstants::LIGHT_SQUARE_COLOR : ChessConstants::DARK_SQUARE_COLOR);
        QString rank = QString::number(row + 1);
        QRect square = squareRect({row, 0});
        painter.drawText(square.left() + padding, square.top() + fm.ascent() + padding, rank);
    }
}

void ChessBoardWidget::mousePressEvent(QMouseEvent* event) {
    bool isLeftClick = (event->button() == Qt::LeftButton);
    if (!isLeftClick || (chessModel && chessModel->isGameOver())) {
        if (selectedSquare.isValid() || dragIndicatorSource.isValid())
            resetInteractionState();
        return;
    }

    if (!chessModel) {
        qWarning() << "MousePress: chessModel is null!";
        return;
    }

    const Position clickedPos = positionFromPoint(event->pos());
    if (!clickedPos.isValid()) {
        if (selectedSquare.isValid() || dragIndicatorSource.isValid())
            resetInteractionState();
        return;
    }

    Piece* clickedPiece = chessModel->getPiece(clickedPos.row, clickedPos.col);
    draggedPiece = nullptr;

    // Case 1: Piece already selected
    if (selectedSquare.isValid()) {
        Position startPos = selectedSquare;
        Piece* selectedPiece = chessModel->getPiece(startPos.row, startPos.col);

        if (!selectedPiece) {
            qWarning() << "Selected square valid but no piece found.";
            resetInteractionState();
            return;
        }

        if (clickedPos == startPos) {
            resetInteractionState();
            return;
        }

        auto validMoves = chessModel->getValidMoves(startPos);
        bool isValidMove = std::find(validMoves.begin(), validMoves.end(), clickedPos) != validMoves.end();

        if (isValidMove) {
            Move move(startPos, clickedPos);
            qDebug() << "Click Move Attempted:" << QString::fromStdString(Utils::moveToSAN(move, *chessModel));
            emit moveAttempted(move);
        } else if (clickedPiece && clickedPiece->isWhite == selectedPiece->isWhite) {
            selectedSquare = clickedPos;
            dragIndicatorSource = {-1, -1};
            dragStartPosition = event->pos();
            draggedPiece = clickedPiece;
            qDebug() << "Selection switched to:" << QString::fromStdString(Utils::positionToString(clickedPos));
            update();
        } else {
            qDebug() << "Selection cancelled (invalid second click).";
            resetInteractionState();
        }
        return;
    }

    // Case 2: No piece selected
    if (clickedPiece && clickedPiece->isWhite == chessModel->isWhiteToMove()) {
        selectedSquare = clickedPos;
        dragIndicatorSource = {-1, -1};
        dragStartPosition = event->pos();
        draggedPiece = clickedPiece;
        qDebug() << "Piece Selected (Click):" << QString::fromStdString(Utils::positionToString(clickedPos));
        update();
    } else if (selectedSquare.isValid() || dragIndicatorSource.isValid()) {
        resetInteractionState();
    }
}

void ChessBoardWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::LeftButton) || !draggedPiece || (chessModel && chessModel->isGameOver()))
        return;

    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    startDrag();
}

void ChessBoardWidget::startDrag() {
    Position startPos = positionFromPoint(dragStartPosition);
    if (!startPos.isValid() || !chessModel) {
        qWarning() << "Invalid drag start position or chessModel is null.";
        resetInteractionState();
        return;
    }

    Piece* piece = chessModel->getPiece(startPos.row, startPos.col);
    if (!piece) {
        qWarning() << "No piece at drag start position.";
        resetInteractionState();
        return;
    }

    if (piece->isWhite != chessModel->isWhiteToMove()) {
        qWarning() << "Attempted to drag opponent's piece.";
        resetInteractionState();
        return;
    }

    selectedSquare = {-1, -1};
    dragIndicatorSource = startPos;
    draggedPiece = nullptr;
    qDebug() << "Drag started from" << QString::fromStdString(Utils::positionToString(startPos));

    QMimeData *mimeData = new QMimeData;
    QByteArray data;
    QDataStream(&data, QIODevice::WriteOnly) << startPos.row << startPos.col;
    mimeData->setData("application/x-chess-move-start", data);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    int sSize = squareSize();
    if (sSize <= 0) {
        qWarning() << "Invalid square size for drag pixmap.";
        resetInteractionState();
        return;
    }

    QPixmap pixmap(sSize, sSize);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    ChessDrawingUtils::drawPiece(painter, piece, pixmap.rect(), sSize);

    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(sSize / 2, sSize / 2));
    update();

    drag->exec(Qt::MoveAction);

    qDebug() << "Drag finished.";
    dragIndicatorSource = {-1, -1};
    selectedSquare = {-1, -1};
    update();
}

void ChessBoardWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (chessModel && !chessModel->isGameOver() && event->mimeData()->hasFormat("application/x-chess-move-start"))
        event->acceptProposedAction();
    else
        event->ignore();
}

void ChessBoardWidget::dragMoveEvent(QDragMoveEvent *event) {
    if (chessModel && !chessModel->isGameOver() && event->mimeData()->hasFormat("application/x-chess-move-start"))
        event->acceptProposedAction();
    else
        event->ignore();
}

void ChessBoardWidget::dropEvent(QDropEvent *event) {
    if (!chessModel || chessModel->isGameOver()) {
        event->ignore();
        resetInteractionState();
        return;
    }

    Position startPos = {-1, -1};
    if (event->mimeData()->hasFormat("application/x-chess-move-start")) {
        QByteArray data = event->mimeData()->data("application/x-chess-move-start");
        QDataStream stream(&data, QIODevice::ReadOnly);
        int row, col;
        if ((stream >> row >> col).status() == QDataStream::Ok)
            startPos = {row, col};
    }

    if (!startPos.isValid()) {
        qWarning() << "Invalid or missing drag start position.";
        event->ignore();
        resetInteractionState();
        return;
    }

    Position endPos = positionFromPoint(event->position().toPoint());
    if (!endPos.isValid()) {
        qWarning() << "Invalid drop position.";
        event->ignore();
        resetInteractionState();
        return;
    }

    Piece* piece = chessModel->getPiece(startPos.row, startPos.col);
    bool isValidDrop = false;
    if (piece && piece->isWhite == chessModel->isWhiteToMove()) {
        auto validMoves = chessModel->getValidMoves(startPos);
        isValidDrop = std::any_of(validMoves.begin(), validMoves.end(),
                                  [&](const Position& p) { return p == endPos; });
    }

    if (isValidDrop) {
        Move move(startPos, endPos);
        qDebug() << "Drag Move Attempted:" << QString::fromStdString(Utils::moveToSAN(move, *chessModel));
        emit moveAttempted(move);
        event->acceptProposedAction();
    } else {
        qDebug() << "Invalid drop move from" << QString::fromStdString(Utils::positionToString(startPos))
                 << "to" << QString::fromStdString(Utils::positionToString(endPos));
        event->ignore();
    }

    resetInteractionState();
}
