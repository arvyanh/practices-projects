#include <iostream>
#include <vector> 
#include <array>
#include <algorithm>
#include <random>

#include "circular_buffer.h"

typedef std::array<std::array<bool, 4>, 4> PieceArray;

//enum of possible tetris pieces
enum Tshape {
    I,
    square,
    Z,
    Zinv,
    L
};

enum class Tdirection {
    up,
    down,
    left,
    right,
    none,
    rotate
};

class TetrisPiece {
private:
    PieceArray piece;
    int distanceToBottom;
public:
    Tshape shape;
    constexpr PieceArray getPiece(Tshape shape) const{
        PieceArray piece = {{{0,0,0,0},
                             {0,0,0,0},
                             {0,0,0,0},
                             {0,0,0,0}}};
        switch (shape) {
            case Tshape::I:
                piece = {{{0,0,0,0},
                          {1,1,1,1},
                          {0,0,0,0},
                          {0,0,0,0}}};
                return piece;
            case Tshape::square:
                piece = {{{0,0,0,0},
                          {0,1,1,0},
                          {0,1,1,0},
                          {0,0,0,0}}};
                return piece;
            case Tshape::Z:
                piece = {{{0,0,0,0},
                          {1,1,0,0},
                          {0,1,1,0},
                          {0,0,0,0}}};
                return piece;
            case Tshape::Zinv:
                piece = {{{0,0,0,0},
                          {0,1,1,0},
                          {1,1,0,0},
                          {0,0,0,0}}};
                return piece;
            case Tshape::L:
                piece = {{{0,0,0,0},
                          {1,1,1,0},
                          {1,0,0,0},
                          {0,0,0,0}}};
                return piece;
            default:
                return piece;
        }
    }

    TetrisPiece(Tshape shape) : shape(shape), piece(getPiece(shape)){
        std::cout << "TetrisPiece constructor called" << std::endl;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++){
                std::cout << piece[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    auto operator[](int index) -> decltype(piece[index]) {
        return piece[index];
    }
    bool rowEmpty(int row);
    bool colEmpty(int col);

    void rotate();
    //try to move the piece in the given direction, return true if possible
    bool move(Tdirection direction);
};

class TetrisBoard {
private:
    //using vector to reduce the cost of moving down the board, reserve enough space
    //std::vector<std::array<bool, 10>> board = std::vector<std::array<bool, 10>>(40);
    CircularBuffer<std::array<bool, 10>> board{20};
    TetrisPiece piece;
    int pieceX;
    int pieceY;
public:
    using Iterator = CircularBuffer<std::array<bool, 10>>;
    TetrisBoard() : piece(Tshape::I), pieceX(3), pieceY(0){}
    bool testRotateCollision();
    bool testCollision(Tdirection direction);
    void printBoardRow(int row);
    void addPiece(TetrisPiece piece);
    void tick(Tdirection direction);
    int checkRowFull(int row);
    int clearLastRow();
    int pieceMove(Tdirection direction);
    int getPieceLeft();
    int getPieceRight();
    int getPieceTop();
    int getPieceDown();
    auto operator[](int index) -> decltype(board[index]) {
        return board[index];
    }
};

class TetrisGame {
public:
    TetrisGame() : score(0){}
    void draw();
    void update();
private:
    TetrisBoard board;
    void gameLoop();
    int score;
};
