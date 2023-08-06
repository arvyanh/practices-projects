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
    }
    return true;
}

//std::array<std::array<bool, 10>, 20> board;
//TetrisPiece currentPiece;

void TetrisBoard::printBoardRow(int row){
    //check if the piece is in this row
    for(auto i = board[row].begin(); i != board[row].end(); i++){
        std::cout << *i;
    }
    std::cout << std::endl;
}

void TetrisBoard::addPiece(TetrisPiece piece){
    piece = std::move(piece);
    pieceX = 3;
    pieceY = 0;
}

int TetrisBoard::getPieceLeft(){return pieceX;}
int TetrisBoard::getPieceRight(){return pieceX + 3;}
int TetrisBoard::getPieceTop(){return pieceY;}
int TetrisBoard::getPieceDown(){return pieceY + 3;}
int TetrisBoard::pieceMove(Tdirection direction){
    if (piece.move(direction)){
        switch (direction) {
            case Tdirection::up:
                if (pieceY == 0) return piece.move(Tdirection::up);
                pieceY--;
                break;
            case Tdirection::down:
                if (pieceY == 16) return piece.move(Tdirection::down);
                pieceY++;
                break;
            case Tdirection::left:
                if (pieceX == 0) return piece.move(Tdirection::left);
                pieceX--;
                break;
            case Tdirection::right:
                if (pieceX == 6) return piece.move(Tdirection::right);
                pieceX++;
                break;
        }
    }
    return true;
}


void TetrisBoard::tick(Tdirection direction){
    pieceMove(direction);
    //reaches the bottom
    if(pieceMove(Tdirection::down)){
        std::array<bool,10>& lastRow = *board.end().ptr;
        if(std::all_of(lastRow.begin(), lastRow.end(), [](bool i){return i == 1;}) ){
            board.dequeue();
            board.enqueue({0,0,0,0,0,0,0,0,0,0});
            Tshape rand_shape = static_cast<Tshape>(rand() % 7);
            addPiece(rand_shape);
        }
        else{
            //if not at the bottom, do nothing
        }
    }

};

