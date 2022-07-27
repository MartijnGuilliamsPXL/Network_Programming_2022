#ifndef ZEESLAG_H
#define ZEESLAG_H

#include <iostream>
#include <vector>
#include "ship.h"

using namespace std;

class Zeeslag
{
public:
    Zeeslag();

    void printBoard(int nr);
    void placeShip(int x1, int y1, int x2, int y2, int board);
private:
    //player 1
    char board1[100];
    int carriers1;
    int battles1;
    int subs1;
    int destr1;
    vector<Ship*> ships1;

    //player 2
    char board2[100];
    int carriers2;
    int battles2;
    int subs2;
    int destr2;
    vector<Ship*> ships2;
};

#endif // ZEESLAG_H
