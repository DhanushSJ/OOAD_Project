#include <iostream>
#include "ChessController.h"

ChessController::ChessController(ChessModel* m, ChessView* v) {
    this->model = m;
    this->view = v;
}

void ChessController::run() {
    int choice;
    view->displayMenu();
    std::cin >> choice;
    
    if (choice == 1) {
        model->setupStartingPosition();
    } else if (choice == 2) {
        std::string fen = view->getFEN();
        model->setupFromFEN(fen);
        
        // Check if user wants to override the FEN active color
        char moveChoice = view->getFirstMoveChoice();
        if (moveChoice == 'w' || moveChoice == 'W') {
            model->setWhiteToMove(true);
        } else if (moveChoice == 'b' || moveChoice == 'B') {
            model->setWhiteToMove(false);
        }
    } else {
        std::cout << "Invalid choice. Using default.\n";
        model->setupStartingPosition();
    }
    
    // Main game loop
    bool gameRunning = true;
    while (gameRunning) {
        view->displayBoard(model);
        
        // Get move from user
        Move move = view->getMove();
        
        // Check if user wants to quit
        if (move.from.row == -1) {
            gameRunning = false;
            continue;
        }
        
        // Show valid moves for the selected piece
        if (model->getPiece(move.from.row, move.from.col) != nullptr) {
            view->displayValidMoves(model, move.from);
        }
        
        // Make the move
        bool moveSuccessful = model->makeMove(move);
        if (!moveSuccessful) {
            view->displayError("Invalid move. Please try again.");
        }
    }
}