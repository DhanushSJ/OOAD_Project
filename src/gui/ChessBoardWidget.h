#ifndef CHESSBOARDWIDGET_H
#define CHESSBOARDWIDGET_H

#include <QWidget>
#include <QPoint>
#include <QRect>

class ChessModel;
class Move;
class Piece;
class QPainter;
class QMouseEvent;
class QPaintEvent;
class QDragEnterEvent;
class QDropEvent;
class QDragMoveEvent;
class QDrag;

#include "model/Position.h"

class ChessBoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChessBoardWidget(ChessModel* model, QWidget *parent = nullptr);
    ~ChessBoardWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void resetInteractionState(bool doUpdate = true);

signals:
    void moveAttempted(const Move& move);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    ChessModel* chessModel;

    int squareSize() const;
    Position positionFromPoint(const QPoint& point) const;
    QRect squareRect(const Position& pos) const;

    QPoint dragStartPosition;
    Piece* draggedPiece = nullptr;
    Position selectedSquare = {-1, -1};
    Position dragIndicatorSource = {-1, -1};

    void startDrag();
};

#endif
