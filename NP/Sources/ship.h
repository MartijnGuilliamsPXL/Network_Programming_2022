#ifndef SHIP_H
#define SHIP_H

#include "math.h"
#include <iostream>

using namespace std;

class Ship
{
public:
    Ship(int x1, int y1, int x2, int y2, int size);
    string shipClass();
private:
    int size;
    int start[2];
    int end[2];
};

#endif // SHIP_H
