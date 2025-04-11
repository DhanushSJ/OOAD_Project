#ifndef KING_H
#define KING_H

#include "Piece.h"
#include "ChessModel.h"

class King : public Piece {
public:
    King(bool white);
    std::vector<Position> getPossibleMoves(Position pos, ChessModel* model) override;
};

#endif // KING_H