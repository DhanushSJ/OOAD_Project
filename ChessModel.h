#ifndef CHESS_MODEL_H
#define CHESS_MODEL_H

#include <string>
#include <vector>
#include "Piece.h"
#include "Position.h"
#include "Move.h"

class ChessModel {
private:
    Piece* board[8][8];
    bool whiteToMove;
    Position* enPassantTarget;
    bool castlingRights[4]; // 0: white kingside, 1: white queenside, 2: black kingside, 3: black queenside
    
public:
    ChessModel();
    ~ChessModel();
    
    void setupStartingPosition();
    void setupFromFEN(const std::string& fen);
    void clearBoard();
    
    Piece* getPiece(int row, int col);
    bool isWhiteToMove();
    void setWhiteToMove(bool white);
    Position* getEnPassantTarget();
    bool getCastlingRight(int index);
    
    std::vector<Position> getValidMoves(Position pos);
    bool isValidMove(Position from, Position to);
    bool makeMove(Move move);
    bool isInCheck(bool white);
    
    // Factory method to create appropriate piece type
    Piece* createPiece(char type, bool isWhite);
};

#endif // CHESS_MODEL_H