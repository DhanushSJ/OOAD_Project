#ifndef ROOK_H
#define ROOK_H

#include "Piece.h"
#include "ChessModel.h"

class Rook : public Piece {
public:
    Rook(bool white);
    std::vector<Position> getPossibleMoves(Position pos, ChessModel* model) override;
};

#endif // ROOK_H