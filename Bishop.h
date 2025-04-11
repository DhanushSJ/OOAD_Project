#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.h"
#include "ChessModel.h"

class Bishop : public Piece {
public:
    Bishop(bool white);
    std::vector<Position> getPossibleMoves(Position pos, ChessModel* model) override;
};

#endif // BISHOP_H