#include "Pawn.h"

Pawn::Pawn(bool white) : Piece('P', white) {}

std::vector<Position> Pawn::getPossibleMoves(Position pos, ChessModel* model) {
    std::vector<Position> moves;
    
    // Function to check if position has an enemy piece (for pawn captures)
    auto hasEnemy = [model, this](int row, int col) -> bool {
        Piece* piece = model->getPiece(row, col);
        return piece != nullptr && piece->isWhite != this->isWhite;
    };
    
    // Function to add a move if valid
    auto addIfValid = [&moves](int row, int col) {
        if (row >= 0 && row < 8 && col >= 0 && col < 8) {
            moves.push_back(Position(row, col));
        }
    };
    
    // Pawns move differently based on color
    int dir = isWhite ? 1 : -1;
    int startRow = isWhite ? 1 : 6;
    
    // Forward one square
    if (model->getPiece(pos.row + dir, pos.col) == nullptr) {
        addIfValid(pos.row + dir, pos.col);
        
        // Forward two squares from starting position
        if (pos.row == startRow && model->getPiece(pos.row + 2 * dir, pos.col) == nullptr) {
            addIfValid(pos.row + 2 * dir, pos.col);
        }
    }
    
    // Capture diagonally
    if (pos.col > 0 && hasEnemy(pos.row + dir, pos.col - 1)) {
        addIfValid(pos.row + dir, pos.col - 1);
    }
    if (pos.col < 7 && hasEnemy(pos.row + dir, pos.col + 1)) {
        addIfValid(pos.row + dir, pos.col + 1);
    }
    
    // En passant
    Position* enPassantTarget = model->getEnPassantTarget();
    if (enPassantTarget != nullptr) {
        if (pos.row == (isWhite ? 4 : 3)) {
            if (pos.col > 0 && enPassantTarget->row == pos.row && enPassantTarget->col == pos.col - 1) {
                addIfValid(pos.row + dir, pos.col - 1);
            }
            if (pos.col < 7 && enPassantTarget->row == pos.row && enPassantTarget->col == pos.col + 1) {
                addIfValid(pos.row + dir, pos.col + 1);
            }
        }
    }
    
    return moves;
}