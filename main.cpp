#include "main.h"
#include <iostream>

void TetrisGame::draw(){
    for (int i = 0; i < 20; i++)
        board.printBoardRow(i);
}
void TetrisGame::update(){
    board.tick(Tdirection::down);
}

int main() {
    TetrisGame game;
    while(true){
        game.draw();
        game.update();

        int dummy;
        std::cin >> dummy;
    }
    return 0;
}
