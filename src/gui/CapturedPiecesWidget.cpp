#include "CapturedPiecesWidget.h"
#include "Constants.h"
#include "DrawingUtils.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QFont>

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
        ChessDrawingUtils::drawPiece(painter, piece, pieceRect, pieceSize);
        x += pieceSize + spacing;
    }
}