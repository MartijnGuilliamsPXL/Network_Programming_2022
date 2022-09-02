#include "service.h"

Service::Service(QObject *parent) :
    QObject(parent)
{
    //default constructor, ignore
}

Service::Service(QObject *parent, QString userid, int lobbynr):
    QObject(parent),
    lobby(lobbynr),
    player1(userid),
    player2(""),
    isStarted(false),
    player2Auto(false),
    standardPrefix("Zeeslag>")
{
    context_p = new zmq::context_t(1);

    //prepare sender
    sender = new zmq::socket_t(*context_p, ZMQ_PUSH);
    sender->connect("tcp://benternet.pxl-ea-ict.be:24041");

    //prepare acknowledgement before new standardprefix
    QString ack = standardPrefix + player1 + '>' + QString(to_string(lobbynr).c_str()) + '>';

    //update prefix to standard
    standardPrefix.append(to_string(lobbynr).c_str());
    standardPrefix.append('>');
    //make sub string
    QString substring = standardPrefix + QString("user>");
    standardPrefix.append("server>");

    //prepare receiver
    receiver = new zmq::socket_t(*context_p, ZMQ_SUB);
    receiver->connect("tcp://benternet.pxl-ea-ict.be:24042");
    receiver->setsockopt(ZMQ_SUBSCRIBE, substring.toStdString().c_str(), substring.size());

    //send the acknowledgement, the lobby is prepared
    sender->send(ack.toStdString().c_str(), ack.size());
}

Service::~Service()
{
    delete sender;
    delete receiver;
    delete context_p;
}

void Service::run()
{
    //preparegame();
    QtConcurrent::run(&Service::preparegame, this);
    //loop();
}

int Service::getLobby()
{
    return lobby;
}

void Service::preparegame(){
    //wait for player 1 ready
    QString receivedmessage = standardReceive();
    while(receivedmessage.section('>', 4, 4) != QString("response") || receivedmessage.section('>', 5, 5) != QString("ready")){
        receivedmessage = standardReceive();
    }
    //asks if player 2 is CPU or Human
    queueCommand(PRINT, QString("Is player 2 Human (or CPU) y/n"));
    queueCommand(REQUEST, QString("bool"));
    sendCommand(PLAYER1);
    if(receiveCommand(BOOL).toInt()){
        QString receivedmessage = standardReceive();
        while(receivedmessage.section('>', 4, 4) != QString("response") || receivedmessage.section('>', 5, 5) != QString("join")){
            receivedmessage = standardReceive();
        }
        player2 = receivedmessage.section('>', 3, 3);
        player2Auto = 0;
        cout << "player 1: " << player1.toStdString() << endl << "player 2: " << player2.toStdString() << endl;
    }
    else{
        player2Auto = 1;
        cout << "player 1: " << player1.toStdString() << endl << "player 2: CPU" << endl;
    }
    playGame();
    emit killme(this);
}

void Service::queueCommand(int command, QString value)
{
    QString toqueue = queuedCommand;
    switch (command){
    case REQUEST:
        toqueue = toqueue + "request>" + value + '>';
        break;
    case PRINT:
        toqueue = toqueue + "print>" + value + '>';
        break;
    case RESPONSE:
        toqueue = toqueue + "response>" + value + '>';
        break;
    }

    queuedCommand = toqueue;
}

void Service::sendCommand(int user)
{
    //by checking on not 1 or not 2 in case of zero both players will be notified
    if(user != PLAYER2) // 1 or both
    {
        QString tosend = standardPrefix + player1 + '>'  + queuedCommand;
        sender->send(tosend.toStdString().c_str(), tosend.size());
    }
    if(user != PLAYER1) // 2 or both
    {
        QString tosend = standardPrefix + player2 + '>' + queuedCommand;
        sender->send(tosend.toStdString().c_str(), tosend.size());
    }
    //reset queue
    queuedCommand = "";
}

QString Service::receiveCommand(int type)
{
    zmq::message_t* datapayload;
    datapayload = new zmq::message_t;
    receiver->recv(datapayload);
    QString response = QString(string((char*) datapayload->data(), datapayload->size()).c_str());
    delete datapayload;
    QString responseValue = response.section('>', 5, 5);
    if(type == 0)
    {
        if(responseValue == QString("0") || responseValue == QString("1"))
        {
            return response.section('>', 5, 5);
        }
        else
        {
            //sending quit message to all players en ending lobby
            queueCommand(PRINT, QString("Error, closing lobby\n\n"));
            emit killme(this);
            //return "error";
        }
    }
    return response.section('>', 5, 5);
}

QString Service::standardReceive(){
    zmq::message_t* datapayload;
    datapayload = new zmq::message_t;
    receiver->recv(datapayload);
    QString response = QString(string((char*) datapayload->data(), datapayload->size()).c_str());
    delete datapayload;
    return response;
}

// function to play game
bool Service::playGame()
{
    int firstPlayer, currentPlayer, toPlay;
    Board * bptr;
    Player * pptr;

    startGame();

    firstPlayer = 1;

    // hold pointers for current player/board objects
    if (firstPlayer==1)
    {
        pptr = &p1;
        bptr = &p2Board;
    }
    else {
        pptr = &p2;
        bptr = &p1Board;
    }

    while(gameCondition()==UNFINISHED)
    {
        if ((*pptr).isPlayerAutomatic())
        {
            getNextMoveAuto(*(bptr));
        }
        else
        {
            printGameState(*(pptr));
            getNextMove(*(bptr));
            //printGameState(*(pptr));
        }

        // switch pointers at the end of each turn
        if ((*pptr).getPlayerNum()==1)
        {
            activeplayer = PLAYER2;
            pptr = &p2;
            bptr = &p1Board;
        }
        else {
            activeplayer = PLAYER1;
            pptr = &p1;
            bptr = &p2Board;
        }
    }

    if (gameCondition()==P1_WIN)
        queueCommand(PRINT, QString(p1.getName().c_str() + QString(" wins!!!")));
    else
        queueCommand(PRINT, QString(p2.getName().c_str() + QString(" wins!!!")));

    queueCommand(PRINT, QString("I hope you enjoyed the Battleship game. Bye!\n\n"));

    sendCommand(ALLPLAYERS);
    return true;

}

// function to perform necessary setup before game is played
void Service::startGame()
{
    std::string nameTemp;
    int autoTemp;

    //set player numbers
    p1.setPlayerNum(1);
    p2.setPlayerNum(2);

    //get player names
    p1.setName("player 1");
    p2.setName("player 2");

    //get automatic statuses
    p1.setAuto(0);
    p2.setAuto(player2Auto);

    //initialize both boards, according to whether the players are automatic
    //initialize p1
    activeplayer = PLAYER1;
    queueCommand(PRINT, QString("how would you like your board to be set? (enter 0 for non-auto, 1 for auto)"));
    queueCommand(REQUEST, QString("bool"));
    sendCommand(activeplayer);
    autoTemp = receiveCommand(BOOL).toInt();
    if (autoTemp)
        initializeBoardAuto(p1Board, true);
    else initializeBoard(p1Board);

    activeplayer = PLAYER2;
    if (p2.isPlayerAutomatic())
        initializeBoardAuto(p2Board, false);
    else
    {
        queueCommand(PRINT, QString("how would you like your board to be set? (enter 0 for non-auto, 1 for auto)"));
        queueCommand(REQUEST, QString("bool"));
        sendCommand(activeplayer);
        autoTemp = receiveCommand(BOOL).toInt();
        if (autoTemp)
            initializeBoardAuto(p2Board, true);
        else initializeBoard(p2Board);

    }
    activeplayer = PLAYER1;
    return;
}

// prints game state, with player's own board exposed and opponent's board
// mostly obscured, save for moves player has made
void Service::printGameState(Player p)
{
    queueCommand(PRINT, QString(p.getName().c_str() + QString("'s GAME STATE:\n\n")));
    if (p.getPlayerNum()==1)
    {
        queueCommand(PRINT, QString("YOUR BOARD: \n"));
        queueCommand(PRINT, QString(p1Board.printPublicBoard()));
        queueCommand(PRINT, QString("YOUR OPPONENT'S BOARD: \n"));
        queueCommand(PRINT, QString(p2Board.printPrivateBoard()));
    }
    else
    {
        queueCommand(PRINT, QString("YOUR BOARD: \n"));
        queueCommand(PRINT, QString(p2Board.printPublicBoard()));
        queueCommand(PRINT, QString("YOUR OPPONENT'S BOARD: \n"));
        queueCommand(PRINT, QString(p1Board.printPrivateBoard()));
    }
    sendCommand(p.getPlayerNum());

    return;
}

// initializes board based on user input
void Service::initializeBoard(Board &b)
{
    int xEntry, yEntry, horizEntry, attemptCount;
    std::string entryTemp;
    for (int i=0; i<NUM_SHIPS; i++)
    {
        attemptCount=0;
        do // check for valid placement of each ship
        {
            b.printPublicBoard();
            if (attemptCount>0)
            queueCommand(PRINT, QString("INVALID ENTRY for that ship! Please try again. \n"));

            queueCommand(PRINT, QString("Please enter location [Letter][Number] for the top/left of your " + QString(SHIP_NAMES[i].c_str()) + " which is length " + QString::number(SHIP_LENGTHS[i]) + ": \n"));
            entryTemp=getSquare();
            xEntry=static_cast<int>(entryTemp[0]);
            yEntry=static_cast<int>(entryTemp[1]);

            queueCommand(PRINT, QString("Please enter 0 if the ship is oriented vertically, 1 if it is oriented horizontally:\n"));
            queueCommand(REQUEST, QString("bool"));
            sendCommand(activeplayer);
            horizEntry = receiveCommand(BOOL).toInt();
            attemptCount++;
        } while (!b.placeShip(i, xEntry-LETTER_CHAR_OFFSET, yEntry-NUMBER_CHAR_OFFSET, horizEntry));

    }

    queueCommand(PRINT, QString("Your starting board: \n"));
    queueCommand(PRINT, QString(b.printPublicBoard()));
    sendCommand(activeplayer);
    return;
}


// initializes a board with random placement of ships on a board
void Service::initializeBoardAuto(Board &b, bool print)
{
    int xEntry, yEntry, horizEntry;

    for (int i=0; i<NUM_SHIPS; i++)
    {
        do // randomize position placements and place ships if possible
        {
            srand(time(NULL));
            xEntry=rand()%10;
            yEntry=rand()%10;
            horizEntry=rand()%2;
        } while (!b.placeShip(i, xEntry, yEntry, horizEntry));
    }

    if (print)
    {
        queueCommand(PRINT, QString("Your starting board: "));
        queueCommand(PRINT, QString(b.printPublicBoard()));
        sendCommand(activeplayer);
    }
    return;
}

// returns enum determining state of game
state Service::gameCondition()
{
    if (p1Board.getNumHits()==TOTAL_SHIP_SPACES)
        return P2_WIN;

    else if (p2Board.getNumHits()==TOTAL_SHIP_SPACES)
        return P1_WIN;

    else
        return UNFINISHED;
}


// function to prompt a player to actively choose which move to make
void Service::getNextMove(Board &b)
{
    int attemptCount=0;
    int xEntry, yEntry;
    bool goodMove=false;
    std::string entryTemp;

    while (!goodMove)
    {
        if (attemptCount>0)
        {
            queueCommand(PRINT, QString("That move has already been attempted. Try again. \n"));
        }


        queueCommand(PRINT, QString("Please enter location [Letter][Number] of desired move:\n"));
        entryTemp=getSquare();
        xEntry=static_cast<int>(entryTemp[0]);
        yEntry=static_cast<int>(entryTemp[1]);

        if (b.getSpaceValue(xEntry-LETTER_CHAR_OFFSET, yEntry-NUMBER_CHAR_OFFSET) != isHIT && b.getSpaceValue(xEntry-LETTER_CHAR_OFFSET, yEntry-NUMBER_CHAR_OFFSET) != isMISS)
        {
            b.recordHit(xEntry-LETTER_CHAR_OFFSET, yEntry-NUMBER_CHAR_OFFSET);
            goodMove=true;
        }
        attemptCount++;
    }

    return;
}


// function to make a random move
void Service::getNextMoveAuto(Board &b)
{
    bool goodMove=false;
    int xEntry, yEntry;

    while (!goodMove)
    {
        // randomly choose next move
        xEntry=rand()%10;
        yEntry=rand()%10;

        if (b.getSpaceValue(xEntry, yEntry) != isHIT && b.getSpaceValue(xEntry, yEntry) != isMISS)
        {
            b.recordHit(xEntry, yEntry);
            goodMove=true;
        }
    }
    return;
}

// input validation for square
std::string Service::getSquare()
{
    std::string retString;
    queueCommand(REQUEST, QString("string"));
    sendCommand(activeplayer);
    retString = receiveCommand(STRING).toStdString();
    bool isGoodInput=false;

    while (!isGoodInput)
    {
        retString[0] = toupper(retString[0]);

        // check for two character entries of letter/number
        if (retString.length() == 2 && (retString[0] >= 65 && retString[0] <= 74) && (retString[1] >= 48 && retString[1] <= 57))
            isGoodInput=true;
        else
        {
            if(retString == "Info")
            {
                queueCommand(PRINT, QString("Winkans: p1Board = " + QString::number(p1Board.getNumHits()) + "/17 hits"));
                queueCommand(PRINT, QString("Winkans: p2Board = " + QString::number(p2Board.getNumHits()) + "/17 hits \n"));
                queueCommand(PRINT, QString("Please enter location [Letter][Number] of desired move:\n"));
            }
            else
            {
                queueCommand(PRINT, QString("Bad input! Please enter location [Letter][Number] of your desired move, with capital letters only:\n"));
            }
            queueCommand(REQUEST, QString("string"));
            sendCommand(activeplayer);
            retString = receiveCommand(STRING).toStdString();
        }
    }

    return retString;
}
