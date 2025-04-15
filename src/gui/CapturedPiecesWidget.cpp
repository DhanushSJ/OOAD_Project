#include "CapturedPiecesWidget.h"
#include "Constants.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QFont>
#include <QColor>
#include <QDebug>

CapturedPiecesWidget::CapturedPiecesWidget(const ChessModel* model, bool showWhiteCaptures, QWidget *parent)
    : QWidget(parent), chessModel(model), showWhiteCaptures(showWhiteCaptures)
{}

CapturedPiecesWidget::~CapturedPiecesWidget() {}

QSize CapturedPiecesWidget::minimumSizeHint() const
{
    // Suggest a minimum width to hold a few pieces and a minimum height for one row
    int pieceSize = 25;
    return QSize(pieceSize * 4, pieceSize + 10);
}

QSize CapturedPiecesWidget::sizeHint() const
{
    // Suggest a size that can comfortably hold maybe 8 pieces in width
    int pieceSize = 30; 
    return QSize(pieceSize * 8 + 20, pieceSize + 20);
}


void CapturedPiecesWidget::drawCapturedPiece(QPainter& painter, Piece* piece, const QRect& targetRect, int pieceSize)
{
     if (!piece || targetRect.isNull() || pieceSize <= 0) return;

    QChar pieceChar = ChessConstants::PIECE_UNICODE_MAP.value(piece->type, '?');
    QString pieceText = QString(pieceChar);
    painter.setFont(QFont("Arial Unicode MS", pieceSize * 0.6));
    QFont currentFont = painter.font();

    QPainterPath textPath;
    textPath.addText(0, 0, currentFont, pieceText);
    QRectF textBoundingRect = textPath.boundingRect();
    qreal dx = targetRect.left() + (targetRect.width() - textBoundingRect.width()) / 2.0 - textBoundingRect.left();
    qreal dy = targetRect.top() + (targetRect.height() - textBoundingRect.height()) / 2.0 - textBoundingRect.top();
    QTransform matrix; matrix.translate(dx, dy);
    textPath = matrix.map(textPath);

    QPainterPathStroker stroker;
    stroker.setWidth(ChessConstants::PIECE_BORDER_THICKNESS);
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);
    QPainterPath borderPath = stroker.createStroke(textPath);

    painter.setPen(Qt::NoPen);
    painter.setBrush(ChessConstants::PIECE_BORDER_COLOR);
    painter.drawPath(borderPath);
    painter.setBrush(piece->isWhite ? ChessConstants::WHITE_PIECE_COLOR : ChessConstants::BLACK_PIECE_COLOR);
    painter.drawPath(textPath);
}


void CapturedPiecesWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (!chessModel) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    const std::vector<Piece*>& capturedPieces = chessModel->getCapturedPieces(showWhiteCaptures);

    // Calculate layout
    int availableWidth = width() - 10; 
    int pieceSize = qMin(height() * ChessConstants::CAPTURED_PIECE_SIZE_FACTOR, 35.0); 
    pieceSize = qMax(pieceSize, 10); 
    int spacing = pieceSize / 5;
    int x = 5; 
    int y = 5; 

    if (availableWidth < pieceSize) return; 

    for (Piece* piece : capturedPieces) {
        if (x + pieceSize > width() - 5) { 
            x = 5;
            y += pieceSize + spacing;
            if (y + pieceSize > height() - 5) break; 
        }
        QRect pieceRect(x, y, pieceSize, pieceSize);
        drawCapturedPiece(painter, piece, pieceRect, pieceSize);
        x += pieceSize + spacing;
    }
}