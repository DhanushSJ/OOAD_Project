#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.h"
#include "ChessModel.h"

class Queen : public Piece {
public:
    Queen(bool white);
    std::vector<Position> getPossibleMoves(Position pos, ChessModel* model) override;
};

#endif // QUEEN_H