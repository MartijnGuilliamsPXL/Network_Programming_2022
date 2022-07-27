#include "zeeslag.h"



Zeeslag::Zeeslag()
{
    for(int i = 0; i < 100; i++){
        board1[i] = 'O';
        board2[i] = 'X';
    }
}

void Zeeslag::printBoard(int nr)
{
    switch (nr) {
    case 1:
        for(int i = 0; i < 100; i++){
            cout << board1[i] << " ";
            if(i%10 == 9){
                cout << endl;
            }
        }
        break;
    case 2:
        for(int i = 0; i < 100; i++){
            cout << board2[i] << " ";
            if(i%10 == 9){
                cout << endl;
            }
        }
        break;
    default:
        cout << "error try board 1 or 2" << endl;
    }
}

void Zeeslag::placeShip(int x1, int y1, int x2, int y2, int board)
{
    if()
}
