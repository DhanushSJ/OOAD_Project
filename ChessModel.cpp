#include <iostream>
#include <cctype>
#include <cmath>
#include "ChessModel.h"
#include "Pawn.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "King.h"

ChessModel::ChessModel() {
    // Initialize the board with nullptr
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = nullptr;
        }
    }
    whiteToMove = true;
    enPassantTarget = nullptr;
    for (int i = 0; i < 4; i++) {
        castlingRights[i] = true;
    }
}

ChessModel::~ChessModel() {
    // Free allocated memory
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != nullptr) {
                delete board[i][j];
            }
        }
    }
    if (enPassantTarget != nullptr) {
        delete enPassantTarget;
    }
}

Piece* ChessModel::createPiece(char type, bool isWhite) {
    switch(type) {
        case 'P': return new Pawn(isWhite);
        case 'N': return new Knight(isWhite);
        case 'B': return new Bishop(isWhite);
        case 'R': return new Rook(isWhite);
        case 'Q': return new Queen(isWhite);
        case 'K': return new King(isWhite);
        default: return nullptr;
    }
}

void ChessModel::setupStartingPosition() {
    clearBoard();
    for (int col = 0; col < 8; col++) {
        board[1][col] = createPiece('P', true); // White pawns
        board[6][col] = createPiece('P', false);  // Black pawns
    }
    board[0][0] = createPiece('R', true);
    board[0][1] = createPiece('N', true);
    board[0][2] = createPiece('B', true);
    board[0][3] = createPiece('Q', true);
    board[0][4] = createPiece('K', true);
    board[0][5] = createPiece('B', true);
    board[0][6] = createPiece('N', true);
    board[0][7] = createPiece('R', true);
    board[7][0] = createPiece('R', false);
    board[7][1] = createPiece('N', false);
    board[7][2] = createPiece('B', false);
    board[7][3] = createPiece('Q', false);
    board[7][4] = createPiece('K', false);
    board[7][5] = createPiece('B', false);
    board[7][6] = createPiece('N', false);
    board[7][7] = createPiece('R', false);
    
    whiteToMove = true;
    for (int i = 0; i < 4; i++) {
        castlingRights[i] = true;
    }
    if (enPassantTarget != nullptr) {
        delete enPassantTarget;
        enPassantTarget = nullptr;
    }
}

void ChessModel::setupFromFEN(const std::string& fen) {
    // Clear the board first
    clearBoard();
    
    int row = 7;
    int col = 0;
    
    // Parse piece placement
    size_t i = 0;
    while (i < fen.length() && fen[i] != ' ') {
        if (fen[i] == '/') {
            row--;
            col = 0;
        } else if (fen[i] >= '1' && fen[i] <= '8') {
            col += fen[i] - '0';
        } else {
            bool isWhite = (fen[i] >= 'A' && fen[i] <= 'Z');
            char type = isWhite ? fen[i] : toupper(fen[i]);
            board[row][col] = createPiece(type, isWhite);
            col++;
        }
        i++;
    }

    i++;
    if (i < fen.length()) {
        whiteToMove = (fen[i] == 'w');
        i += 2;
    }
    
    // Parse castling availability
    for (int j = 0; j < 4; j++) {
        castlingRights[j] = false;
    }
    
    while (i < fen.length() && fen[i] != ' ') {
        if (fen[i] == 'K') castlingRights[0] = true;
        else if (fen[i] == 'Q') castlingRights[1] = true;
        else if (fen[i] == 'k') castlingRights[2] = true;
        else if (fen[i] == 'q') castlingRights[3] = true;
        i++;
    }
    
    i++;
    
    // Parse en passant target square
    if (i < fen.length() && fen[i] != '-') {
        int enPassantCol = fen[i] - 'a';
        i++;
        int enPassantRow = 8 - (fen[i] - '0');
        if (enPassantTarget != nullptr) {
            delete enPassantTarget;
        }
        enPassantTarget = new Position(enPassantRow, enPassantCol);
        i++;
    } else if (i < fen.length()) {
        i++; // Skip the '-'
    }
}

void ChessModel::clearBoard() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != nullptr) {
                delete board[i][j];
                board[i][j] = nullptr;
            }
        }
    }
}

Piece* ChessModel::getPiece(int row, int col) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        return board[row][col];
    }
    return nullptr;
}

bool ChessModel::isWhiteToMove() {
    return whiteToMove;
}

void ChessModel::setWhiteToMove(bool white) {
    whiteToMove = white;
}

Position* ChessModel::getEnPassantTarget() {
    return enPassantTarget;
}

bool ChessModel::getCastlingRight(int index) {
    if (index >= 0 && index < 4) {
        return castlingRights[index];
    }
    return false;
}

// Get all valid moves for the piece at the given position
std::vector<Position> ChessModel::getValidMoves(Position pos) {
    Piece* piece = getPiece(pos.row, pos.col);
    if (piece == nullptr || piece->isWhite != whiteToMove) {
        return std::vector<Position>();
    }
    
    return piece->getPossibleMoves(pos, this);
}

// Check if the move is valid for the piece at the starting position
bool ChessModel::isValidMove(Position from, Position to) {
    std::vector<Position> validMoves = getValidMoves(from);
    for (Position& validMove : validMoves) {
        if (validMove.row == to.row && validMove.col == to.col) {
            return true;
        }
    }
    return false;
}

// Make a move on the board
bool ChessModel::makeMove(Move move) {
    Position from = move.from;
    Position to = move.to;
    
    if (!from.isValid() || !to.isValid()) {
        return false;
    }
    
    Piece* piece = getPiece(from.row, from.col);
    if (piece == nullptr || piece->isWhite != whiteToMove) {
        return false;
    }
    
    if (!isValidMove(from, to)) {
        return false;
    }
    
    // Handle en passant capture
    if (piece->type == 'P' && enPassantTarget != nullptr && 
        to.row == enPassantTarget->row && to.col == enPassantTarget->col) {
        // Remove the captured pawn
        int capturedRow = piece->isWhite ? to.row + 1 : to.row - 1;
        delete board[capturedRow][to.col];
        board[capturedRow][to.col] = nullptr;
    }
    
    // Set new en passant target (for pawn double moves)
    if (enPassantTarget != nullptr) {
        delete enPassantTarget;
        enPassantTarget = nullptr;
    }
    
    if (piece->type == 'P' && abs(from.row - to.row) == 2) {
        int enPassantRow = piece->isWhite ? from.row + 1 : from.row - 1;
        enPassantTarget = new Position(enPassantRow, from.col);
    }
    
    // Handle castling
    if (piece->type == 'K' && abs(from.col - to.col) == 2) {
        // Kingside castling
        if (to.col > from.col) {
            Piece* rook = getPiece(from.row, 7);
            if (rook != nullptr && rook->type == 'R') {
                board[from.row][5] = rook;
                board[from.row][7] = nullptr;
            }
        }
        // Queenside castling
        else {
            Piece* rook = getPiece(from.row, 0);
            if (rook != nullptr && rook->type == 'R') {
                board[from.row][3] = rook;
                board[from.row][0] = nullptr;
            }
        }
    }
    
    // Update castling rights
    if (piece->type == 'K') {
        if (piece->isWhite) {
            castlingRights[0] = false;
            castlingRights[1] = false;
        } else {
            castlingRights[2] = false;
            castlingRights[3] = false;
        }
    }
    
    if (piece->type == 'R') {
        if (piece->isWhite) {
            if (from.row == 0 && from.col == 0) castlingRights[1] = false;
            if (from.row == 0 && from.col == 7) castlingRights[0] = false;
        } else {
            if (from.row == 7 && from.col == 0) castlingRights[3] = false;
            if (from.row == 7 && from.col == 7) castlingRights[2] = false;
        }
    }
    
    // Handle pawn promotion
    if (piece->type == 'P' && (to.row == 0 || to.row == 7)) {
        delete piece;
        piece = createPiece('Q', piece->isWhite); // Default promote to queen
    }
    
    // Delete captured piece if exists
    if (board[to.row][to.col] != nullptr) {
        delete board[to.row][to.col];
    }
    
    // Move the piece
    board[to.row][to.col] = piece;
    board[from.row][from.col] = nullptr;
    
    // Mark the piece as moved
    piece->hasMoved = true;
    
    // Switch turns
    whiteToMove = !whiteToMove;
    
    return true;
}

bool ChessModel::isInCheck(bool white) {
    // Find the king
    Position kingPos(-1, -1);
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece* piece = getPiece(row, col);
            if (piece != nullptr && piece->type == 'K' && piece->isWhite == white) {
                kingPos = Position(row, col);
                break;
            }
        }
        if (kingPos.row != -1) break;
    }
    
    if (kingPos.row == -1) return false; // No king found
    
    // Check if any opponent piece can attack the king
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece* piece = getPiece(row, col);
            if (piece != nullptr && piece->isWhite != white) {
                std::vector<Position> moves = piece->getPossibleMoves(Position(row, col), this);
                for (Position& move : moves) {
                    if (move.row == kingPos.row && move.col == kingPos.col) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}