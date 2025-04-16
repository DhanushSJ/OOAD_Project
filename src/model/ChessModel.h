#ifndef CHESS_MODEL_H
#define CHESS_MODEL_H

#include <string>
#include <vector>
#include <optional>
#include "model/pieces/Piece.h"
#include "Position.h"
#include "Move.h"

class Piece;

class ChessModel {
private:
    Piece* board[8][8];
    bool whiteToMove;
    Position* enPassantTarget;
    bool castlingRights[4]; // 0: white kingside, 1: white queenside, 2: black kingside, 3: black queenside

    // Game State
    bool isCheckmate = false;
    bool isStalemate = false;
    std::vector<Move> currentValidMoves;
    std::vector<Piece*> capturedByWhite;
    std::vector<Piece*> capturedByBlack;
    std::vector<Move> moveHistory;      

    // Private Helper Methods
    void clearBoard();
    Piece* createPiece(char type, bool isWhite);
    Position findKing(bool white) const;
    bool isSquareAttacked(Position square, bool byWhite) const;
    void updateCastlingRights(const Move& move, Piece* movedPiece, Piece* capturedPiece);
    void updateEnPassantTarget(const Move& move, Piece* movedPiece);
    bool isMoveLegal(const Move& move);
    void updateCurrentValidMoves();
    void updateGameStatus();
    void clearCapturedPieces();
    std::string generateFEN() const;


public:
    ChessModel();
    ~ChessModel();

    void setupStartingPosition();
    void setupFromFEN(const std::string& fen);

    Piece* getPiece(int row, int col) const;
    bool isWhiteToMove() const;
    void setWhiteToMove(bool white);
    Position* getEnPassantTarget() const;
    bool getCastlingRight(int index) const;
    bool getIsCheckmate() const { return isCheckmate; }
    bool getIsStalemate() const { return isStalemate; }
    bool isGameOver() const { return isCheckmate || isStalemate; }
    bool isInCheck() const;
    const std::vector<Piece*>& getCapturedPieces(bool capturedByWhitePlayer) const; 
    const std::vector<Move>& getMoveHistory() const { return moveHistory; } 
    std::vector<Position> getValidMoves(Position pos) const;
    bool makeMove(const Move& move);
    std::string getCurrentFEN() const;
};

#endif // CHESS_MODEL_H