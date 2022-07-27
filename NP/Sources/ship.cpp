#include "ship.h"


Ship::Ship(int x1, int y1, int x2, int y2, int size):
    start{x1, y1},
    end{x2, y2},
    size(size)
{
}

string Ship::shipClass()
{
    switch (size) {
    case 2:
        return "destroyer";
    case 3:
        return "duikboot";
    case 4:
        return "slagschip";
    case 6:
        return "vliegdekschip";
    }
}



