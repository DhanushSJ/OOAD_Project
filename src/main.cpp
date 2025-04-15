#include <QApplication> // Qt Application entry point
#include "gui/MainWindow.h"   // Our main window class
#include "model/ChessModel.h"   // Include model for console mode
#include "gui/ChessView.h"    // Include view for console mode
#include "controller/ChessController.h" // Include controller for console mode
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[])
{
    bool consoleMode = false;
    std::vector<std::string> args(argv + 1, argv + argc); // Get command line arguments

    // Check for a simple "--console" flag
    for (const std::string& arg : args) {
        if (arg == "--console" || arg == "-c") {
            consoleMode = true;
            break;
        }
    }

    if (consoleMode) {
        std::cout << "Running in Console Mode...\n";
        ChessModel model;
        ChessView view;
        ChessController controller(&model, &view);
        controller.run(); // Start the console game loop
        return 0; // Exit after console game finishes
    } else {
        // --- GUI Mode ---
        // Create the Qt Application instance
        QApplication app(argc, argv);

        // Create and show the main window (which creates its own model)
        MainWindow mainWindow;
        mainWindow.show();

        // Start the Qt event loop
        return app.exec();
    }
}