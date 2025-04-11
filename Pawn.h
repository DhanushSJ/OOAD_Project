#ifndef PAWN_H
#define PAWN_H

#include "Piece.h"
#include "ChessModel.h"

class Pawn : public Piece {
public:
    Pawn(bool white);
    std::vector<Position> getPossibleMoves(Position pos, ChessModel* model) override;
};

#endif // PAWN_H