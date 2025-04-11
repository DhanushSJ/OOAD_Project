#include <iostream>
#include "ChessModel.h"
#include "ChessView.h"
#include "ChessController.h"

int main() {
    ChessModel model;
    ChessView view;
    ChessController controller(&model, &view);
    
    controller.run();
    
    return 0;
}