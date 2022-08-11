#include <string>

#ifndef PLAYER_H
#define PLAYER_H

class Player
{
	private:
		std::string name;
		bool isAutomaticPlayer;
		int playerNumber;
	public:
		Player();
		Player(std::string n, bool a, int num);
		Player(const Player &oldPlayer);
		Player& operator=(const Player &obj);
		~Player() {return;};
		std::string getName();
		bool isPlayerAutomatic();
		int getPlayerNum();
		void setName(std::string n);
		void setAuto(bool a);
		void setPlayerNum(int n);
};

#endif
