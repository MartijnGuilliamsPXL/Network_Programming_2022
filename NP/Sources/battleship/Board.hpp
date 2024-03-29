#include "Ship.hpp"
#include "Constants.hpp"
#include <vector>
#include <QString>

#ifndef BOARD_H
#define BOARD_H

using namespace std;

class Board
{
	private:
		char gameBoard[BOARD_DIM][BOARD_DIM];
		std::vector<Ship> shipVec;

	public:
		Board();
		Board(const Board &oldBoard);
		Board& operator=(const Board &right);
		~Board() {return;}; // standard destructor - no dynamically memory
		int getNumHits();
        QString printPrivateBoard();
        QString printPublicBoard();
		char getSpaceValue(int x, int y);
		bool recordHit(int x, int y);
		bool placeShip(int shipNum, int x, int y, bool isHorizontal);
};

#endif




