#ifndef KNIGHT_H
#define KNIGHT_H

#include "Piece.h"
#include "ChessModel.h"

class Knight : public Piece {
public:
    Knight(bool white);
    std::vector<Position> getPossibleMoves(Position pos, ChessModel* model) override;
};

#endif // KNIGHT_H