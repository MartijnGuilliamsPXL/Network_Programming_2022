#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QString>
#include <vector>
#include <zmq.hpp>
#include <QtConcurrent>
#include <iostream>

#include "battleship/Board.hpp"
#include "battleship/Player.hpp"
#include "battleship/utils.hpp"

#include <cstdlib>
#include <ctime>


enum state {P1_WIN, P2_WIN, UNFINISHED};

using namespace std;

class Service : public QObject
{
    Q_OBJECT
public:
    explicit Service(QObject *parent = nullptr);

    Service(QObject *parent, QString userid, int lobbynr);
    ~Service();
    void run();
    int getLobby();

signals:
    void killme(Service* toKill);
private:
    ////////////////
    /// //in
    /// naam>servicenaam>lobby>user>USRID>command>val>...
    /// //out
    /// naam>servicenaam>lobby>server>player>command>val...
    ///
    /// player USRID/ 0 => allebei



    int lobby;
    zmq::socket_t* sender;
    zmq::socket_t* receiver;
    zmq::context_t* context_p;

    QString standardPrefix;
    QString player1;
    QString player2;
    bool isStarted;
    bool player2Auto;


    void setupLoop();
    int processSetupCommand(QString command);
    void sendCommand(QString command);
    QString receiveCommand(int player, QString desiredcommand);

    // Game
    /////////////////////
    /// command types:
    /// binval: 0 or 1
    /// coor: x,y
private:
    Player p1;
    Board p1Board;
    Player p2;
    Board p2Board;
    int activeplayer;
public:

    bool playGame();
    void startGame();
    void initializeBoard(Board &b);
    void initializeBoardAuto(Board &b, bool print=true);
    state gameCondition();
    void printGameState(Player p);
    void getNextMove(Board &b);
    void getNextMoveAuto(Board &b);
    std::string getSquare();
    //void switchPlayers(std::string playerFrom, std::string playerTo);

};

#endif // SERVICE_H
