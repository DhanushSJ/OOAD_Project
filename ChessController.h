#ifndef CHESS_CONTROLLER_H
#define CHESS_CONTROLLER_H

#include "ChessModel.h"
#include "ChessView.h"

class ChessController {
private:
    ChessModel* model;
    ChessView* view;
    
public:
    ChessController(ChessModel* m, ChessView* v);
    void run();
};

#endif // CHESS_CONTROLLER_H