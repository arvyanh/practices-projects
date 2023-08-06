#include "main.h"

bool TetrisPiece::rowEmpty(int row){
    for (int i = 0; i < 4; i++){
        if (piece[row][i] == 1){
            return false;
        }
    }
    return true;
}

bool TetrisPiece::colEmpty(int col){
    for (int i = 0; i < 4; i++){
        if (piece[i][col] == 1){
            return false;
        }
    }
    return true;
}

//rotate the piece clockwise
void TetrisPiece::rotate(){
    PieceArray rotatedPiece;
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            rotatedPiece[i][j] = piece[3-j][i];
        }
    }
    piece = std::move(rotatedPiece);
}

//try to move the piece in the given direction, return true if possible
bool TetrisPiece::move(Tdirection direction){
    switch (direction) {
        case Tdirection::up:
            if (!rowEmpty(0)) return false;
            for (int i = 0; i < 3; i++) piece[i] = std::move(piece[i+1]);
            piece[3] = {0,0,0,0};
            break;
        case Tdirection::down:
            if (!rowEmpty(3)) return false;
            for (int i = 3; i > 0; i--) piece[i] = std::move(piece[i-1]);
            piece[0] = {0,0,0,0};
            break;
        case Tdirection::left:
            if (!colEmpty(0)) return false;
            for (int i = 0; i < 3; i++){
                for (int j = 0; j < 4; j++){
                    piece[j][i] = piece[j][i+1];
                }
            }
            for (int i = 0; i < 4; i++) piece[i][3] = 0;
            break;
        case Tdirection::right:
            if (!colEmpty(3)) return false;
            for (int i = 3; i > 0; i--){
                for (int j = 0; j < 4; j++){
                    piece[j][i] = piece[j][i-1];
                }
            }
            for (int i = 0; i < 4; i++) piece[i][0] = 0;
            break;
        case Tdirection::none:
            break;
        case Tdirection::rotate:
            rotate();
            break;
    }
    return true;
}

//std::array<std::array<bool, 10>, 20> board;
//TetrisPiece currentPiece;

void TetrisBoard::printBoardRow(int row){
    if (row >= pieceY && row < pieceY + 4){
        for(int i = 0; i < 10; i++){
            if (i >= pieceX && i < pieceX + 4){
                std::cout << (piece[row - pieceY][i - pieceX] || board[row][i]);
            }
            else{
                std::cout << board[row][i];
            }
        }
    }
    else{
        for(int i = 0; i < 10; i++){
            std::cout << board[row][i];
        }
    }
    std::cout << std::endl;
}

void TetrisBoard::addPiece(TetrisPiece piece_in){
    piece = std::move(piece_in);
    pieceX = 3;
    pieceY = 0;
}

int TetrisBoard::getPieceLeft(){return pieceX;}
int TetrisBoard::getPieceRight(){return pieceX + 3;}
int TetrisBoard::getPieceTop(){return pieceY;}
int TetrisBoard::getPieceDown(){return pieceY + 3;}

bool TetrisBoard::testCollision(Tdirection direction){
    int dx = 0,dy = 0;
    switch (direction) {
        case Tdirection::up:
            dy = -1; break;
        case Tdirection::down:
            dy = 1; break;
        case Tdirection::left:
            dx = -1; break;
        case Tdirection::right:
            dx = 1; break;
        //ignore rotation
        default: break;
    }

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if (piece[i][j] == 1){
                if (board[pieceY + i + dy][pieceX + j + dx] == 1){
                    return true;
                }
            }
        }
    }
    return false;
}

bool TetrisBoard::testRotateCollision(){
    TetrisPiece rotatedPiece = piece;
    rotatedPiece.rotate();
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if (rotatedPiece[i][j] == 1){
                if (board[pieceY + i][pieceX + j] == 1){
                    return true;
                }
            }
        }
    }
    return false;
}

int TetrisBoard::pieceMove(Tdirection direction){
    switch (direction) {
        case Tdirection::up:
            if (testCollision(direction)) return false;
            if (pieceY == 0) return piece.move(Tdirection::up);
            pieceY--;
            break;
        case Tdirection::down:
            if (testCollision(direction)) return false;
            if (pieceY == 16) return piece.move(Tdirection::down);
            pieceY++;
            break;
        case Tdirection::left:
            if (testCollision(direction)) return false;
            if (pieceX == 0) return piece.move(Tdirection::left);
            pieceX--;
            break;
        case Tdirection::right:
            if (testCollision(direction)) return false;
            if (pieceX == 6) return piece.move(Tdirection::right);
            pieceX++;
            break;
        case Tdirection::none:
            break;
        case Tdirection::rotate:
            if (testRotateCollision()) return false;
            piece.rotate();
            break;
    }
    return true;
}

void TetrisBoard::tick(Tdirection direction){
    //reaches the bottom
    if(!pieceMove(direction)){
        std::cout << "piece reached bottom" << std::endl;
        //add piece to board
        for (int i = 0; i < 4; i++){
            for (int j = 0; j < 4; j++){
                board[pieceY + i][pieceX + j] = piece[i][j] || board[pieceY + i][pieceX + j];
            }
        }

        //!!!!!!!!!!
        //This only check the last few rows
        //TODO: check all rows
        while(true){
            std::array<bool,10>& lastRow = *board.end().ptr;
            if(std::all_of(lastRow.begin(), lastRow.end(), [](bool i){return i == 1;}) ){
                board.dequeue();
                board.enqueue({0,0,0,0,0,0,0,0,0,0});
            }
            else{
                break;
            }
        }
        Tshape rand_shape = static_cast<Tshape>(rand() % 5);
        addPiece(rand_shape);
    }

};
