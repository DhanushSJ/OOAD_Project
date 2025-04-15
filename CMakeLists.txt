cmake_minimum_required(VERSION 3.16) # Qt 6 generally needs CMake 3.16+

project(ChessQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) # Use C++17 standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON) # Automatically run Meta-Object Compiler for Q_OBJECT
set(CMAKE_AUTORCC ON) # Automatically run Resource Compiler (if needed later)
set(CMAKE_AUTOUIC ON) # Automatically run UI Compiler (if needed later)

# Find the Qt6 package and its components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Define source files with their new paths
set(PROJECT_SOURCES
    src/main.cpp
    # GUI
    src/gui/CapturedPiecesWidget.cpp
    src/gui/ChessBoardWidget.cpp
    src/gui/ChessView.cpp          # Path updated based on tree
    src/gui/MainWindow.cpp
    # Model
    src/model/ChessModel.cpp
    src/model/Move.cpp
    src/model/Position.cpp
    # Model/Pieces
    src/model/pieces/Bishop.cpp
    src/model/pieces/King.cpp
    src/model/pieces/Knight.cpp
    src/model/pieces/Pawn.cpp
    src/model/pieces/Piece.cpp
    src/model/pieces/Queen.cpp
    src/model/pieces/Rook.cpp
    # Controller
    src/controller/ChessController.cpp
    # Core/Utils
    src/core/Utils.cpp
)

# Define header files (Optional but good practice for IDEs and AUTOMOC)
set(PROJECT_HEADERS
    # GUI
    src/gui/CapturedPiecesWidget.h
    src/gui/ChessBoardWidget.h
    src/gui/ChessView.h        
    src/gui/Constants.h        
    src/gui/MainWindow.h
    # Model
    src/model/ChessModel.h
    src/model/Move.h
    src/model/Position.h
    # Model/Pieces
    src/model/pieces/Bishop.h
    src/model/pieces/King.h
    src/model/pieces/Knight.h
    src/model/pieces/Pawn.h
    src/model/pieces/Piece.h
    src/model/pieces/Queen.h
    src/model/pieces/Rook.h
    # Controller
    src/controller/ChessController.h
    # Core/Utils
    src/core/Utils.h
)

# Define the executable target
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# === Include Directories ===
# Tell the compiler where to find headers.
# This allows you to use #include "gui/MainWindow.h", etc. from any .cpp file.
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# === Link Libraries ===
# Link the executable against the required Qt modules
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

# === Installation ===
# Optional: Install executable to a 'bin' directory relative to CMAKE_INSTALL_PREFIX
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
