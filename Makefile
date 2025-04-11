CXX = g++
CXXFLAGS = -std=c++11 -Wall
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TARGET = $(BIN_DIR)/chess

# Phony targets
.PHONY: all clean

# Default target
all: $(TARGET)

# Create directories
$(OBJ_DIR):
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(BIN_DIR):
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
# Target rule
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Object file rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Header dependencies
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/ChessModel.h $(SRC_DIR)/ChessView.h $(SRC_DIR)/ChessController.h
$(OBJ_DIR)/Position.o: $(SRC_DIR)/Position.cpp $(SRC_DIR)/Position.h
$(OBJ_DIR)/Move.o: $(SRC_DIR)/Move.cpp $(SRC_DIR)/Move.h $(SRC_DIR)/Position.h
$(OBJ_DIR)/Piece.o: $(SRC_DIR)/Piece.cpp $(SRC_DIR)/Piece.h $(SRC_DIR)/Position.h
$(OBJ_DIR)/Pawn.o: $(SRC_DIR)/Pawn.cpp $(SRC_DIR)/Pawn.h $(SRC_DIR)/Piece.h $(SRC_DIR)/ChessModel.h
$(OBJ_DIR)/Knight.o: $(SRC_DIR)/Knight.cpp $(SRC_DIR)/Knight.h $(SRC_DIR)/Piece.h $(SRC_DIR)/ChessModel.h
$(OBJ_DIR)/Bishop.o: $(SRC_DIR)/Bishop.cpp $(SRC_DIR)/Bishop.h $(SRC_DIR)/Piece.h $(SRC_DIR)/ChessModel.h
$(OBJ_DIR)/Rook.o: $(SRC_DIR)/Rook.cpp $(SRC_DIR)/Rook.h $(SRC_DIR)/Piece.h $(SRC_DIR)/ChessModel.h
$(OBJ_DIR)/Queen.o: $(SRC_DIR)/Queen.cpp $(SRC_DIR)/Queen.h $(SRC_DIR)/Piece.h $(SRC_DIR)/ChessModel.h
$(OBJ_DIR)/King.o: $(SRC_DIR)/King.cpp $(SRC_DIR)/King.h $(SRC_DIR)/Piece.h $(SRC_DIR)/ChessModel.h
$(OBJ_DIR)/ChessModel.o: $(SRC_DIR)/ChessModel.cpp $(SRC_DIR)/ChessModel.h $(SRC_DIR)/Piece.h $(SRC_DIR)/Position.h $(SRC_DIR)/Move.h $(SRC_DIR)/Pawn.h $(SRC_DIR)/Knight.h $(SRC_DIR)/Bishop.h $(SRC_DIR)/Rook.h $(SRC_DIR)/Queen.h $(SRC_DIR)/King.h
$(OBJ_DIR)/ChessView.o: $(SRC_DIR)/ChessView.cpp $(SRC_DIR)/ChessView.h $(SRC_DIR)/ChessModel.h $(SRC_DIR)/Move.h
$(OBJ_DIR)/ChessController.o: $(SRC_DIR)/ChessController.cpp $(SRC_DIR)/ChessController.h $(SRC_DIR)/ChessModel.h $(SRC_DIR)/ChessView.h

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)