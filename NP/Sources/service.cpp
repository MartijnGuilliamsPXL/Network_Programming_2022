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
    standardPrefix("name>servicename>")
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

    srand(time(NULL));
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
    while(receivedmessage.section('>', 5, 5) != QString("response") || receivedmessage.section('>', 6, 6) != QString("ready")){
        receivedmessage = standardReceive();
    }
    //asks if player 2 is CPU or Human
    queueCommand(PRINT, QString("Is player 2 Human (or CPU) y/n"));
    queueCommand(REQUEST, QString("bool"));
    sendCommand(PLAYER1);
    if(receiveCommand().toInt()){
        QString receivedmessage = standardReceive();
        while(receivedmessage.section('>', 5, 5) != QString("response") || receivedmessage.section('>', 6, 6) != QString("join")){
            receivedmessage = standardReceive();
        }
        player2 = receivedmessage.section('>', 4, 4);
        player2Auto = 0;
        cout << "player 1: " << player1.toStdString() << endl << "player 2: " << player2.toStdString() << endl;
    }
    else{
        player2Auto = 1;
        cout << "player 1: " << player1.toStdString() << endl << "player 2: CPU" << endl;
    }
    playGame();
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

QString Service::receiveCommand()
{
    zmq::message_t* datapayload;
    datapayload = new zmq::message_t;
    receiver->recv(datapayload);
    QString response = QString(string((char*) datapayload->data(), datapayload->size()).c_str());
    delete datapayload;
    return response.section('>', 6, 6);
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
            /*
            if (!p1.isPlayerAutomatic() && !p2.isPlayerAutomatic())
            {
                //switchPlayers(p1.getName(), p2.getName());
            }
            */
            pptr = &p2;
            bptr = &p1Board;
        }
        else {
            activeplayer = PLAYER1;
            /*
            if (!p1.isPlayerAutomatic() && !p2.isPlayerAutomatic())
            {
                //switchPlayers(p2.getName(), p1.getName());
            }
            */
            pptr = &p1;
            bptr = &p2Board;
        }
    }

    if (gameCondition()==P1_WIN)
        queueCommand(PRINT, QString(p1.getName().c_str() + QString(" wins!!!")));
        //sendCommand(QString(p1.getName().c_str() + QString(" wins!!!")));
        //std::cout<<p1.getName()<<" wins!!!"<<std::endl;
    else
        queueCommand(PRINT, QString(p2.getName().c_str() + QString(" wins!!!")));
        //sendCommand(QString(p2.getName().c_str() + QString(" wins!!!")));
        //std::cout<<p2.getName()<<" wins!!!"<<std::endl;

    queueCommand(PRINT, QString("I hope you enjoyed the Battleship game. Bye!\n\n"));
    //sendCommand(QString("I hope you enjoyed the Battleship game. Bye!\n\n"));
    //std::cout<<"I hope you enjoyed the Battleship game. Bye!\n\n";

    sendCommand(activeplayer);
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
    autoTemp = receiveCommand().toInt();
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
        autoTemp = receiveCommand().toInt();
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
    //sendCommand(QString(p.getName().c_str()) + QString("'s GAME STATE:\n\n"));
    //std::cout<<p.getName()<<"'s GAME STATE:\n\n";
    if (p.getPlayerNum()==1)
    {
        queueCommand(PRINT, QString("YOUR BOARD: \n"));
        //sendCommand(QString("YOUR BOARD: \n"));
        //std::cout<<"YOUR BOARD: \n";
        queueCommand(PRINT, QString(p1Board.printPublicBoard()));
        //sendCommand(QString(p1Board.printPublicBoard().c_str()));
        //p1Board.printPublicBoard();
        queueCommand(PRINT, QString("YOUR OPPONENT'S BOARD: \n"));
        //sendCommand(QString("YOUR OPPONENT'S BOARD: \n"));
        //std::cout<<"YOUR OPPONENT'S BOARD: \n";
        queueCommand(PRINT, QString(p2Board.printPrivateBoard()));
        //sendCommand(QString(p2Board.printPrivateBoard().c_str()));
        //p2Board.printPrivateBoard();
        //std::cout<<std::endl<<std::endl;
    }
    else
    {
        queueCommand(PRINT, QString("YOUR BOARD: \n"));
        //sendCommand(QString("YOUR BOARD: \n"));
        //std::cout<<"YOUR BOARD: \n";
        queueCommand(PRINT, QString(p2Board.printPublicBoard()));
        //sendCommand(QString(p2Board.printPublicBoard().c_str()));
        //p2Board.printPublicBoard();
        queueCommand(PRINT, QString("YOUR OPPONENT'S BOARD: \n"));
        //sendCommand(QString("YOUR OPPONENT'S BOARD: \n"));
        //std::cout<<"YOUR OPPONENT'S BOARD: \n";
        queueCommand(PRINT, QString(p1Board.printPrivateBoard()));
        //sendCommand(QString(p1Board.printPrivateBoard().c_str()));
        //p1Board.printPrivateBoard();
        //std::cout<<std::endl<<std::endl;
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
            //sendCommand(QString("INVALID ENTRY for that ship! Please try again. \n"));
            //std::cout<<"INVALID ENTRY for that ship! Please try again. \n";

            queueCommand(PRINT, QString("Please enter location [Letter][Number] for the top/left of your " + QString(SHIP_NAMES[i].c_str()) + " which is length " + QString(char(SHIP_LENGTHS[i])) + ": \n"));
            //sendCommand(QString("Please enter location [Letter][Number] for the top/left of your " + QString(SHIP_NAMES[i].c_str()) + " which is length " + QString(char(SHIP_LENGTHS[i])) + ": \n"));
            //std::cout<<"Please enter location [Letter][Number] for the "<<
            //            "top/left of your "<<SHIP_NAMES[i]<<" which is length "
            //            <<SHIP_LENGTHS[i]<<": \n";
            entryTemp=getSquare();
            xEntry=static_cast<int>(entryTemp[0]);
            yEntry=static_cast<int>(entryTemp[1]);

            queueCommand(PRINT, QString("Please enter 0 if the ship is oriented vertically, 1 if it is oriented horizontally:\n"));
            //sendCommand(QString("Please enter 0 if the ship is oriented vertically, 1 if it is oriented horizontally:\n"));
            //std::cout<<"Please enter 0 if the ship is oriented vertically, "
            //            <<"1 if it is oriented horizontally:\n";

            queueCommand(PRINT, QString("prestuff>print>geef 0 als het schip verticaal moet of 1 voor horizontaal>binval>req>"));
            //sendCommand(QString("prestuff>print>geef 0 als het schip verticaal moet of 1 voor horizontaal>binval>req>"));
            queueCommand(REQUEST, QString("binval"));
            sendCommand(activeplayer);
            horizEntry = receiveCommand().toInt();

            attemptCount++;
        } while (!b.placeShip(i, xEntry-LETTER_CHAR_OFFSET,
                        yEntry-NUMBER_CHAR_OFFSET, horizEntry));

    }

    queueCommand(PRINT, QString("Your starting board: \n"));
    //sendCommand(QString("Your starting board: \n"));
    //std::cout<<"Your starting board: \n";

    queueCommand(PRINT, QString(b.printPublicBoard()));
    //sendCommand(QString(b.printPublicBoard().c_str()));
    //b.printPublicBoard();
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
        /*std::cout<<"Your starting board: \n";
        if(activeplayer == 1){
            QString prepend = player1 + QString(">board>");
            //sendCommand(prepend + b.printPublicBoard().c_str() + ">");
        }
        else{
            QString prepend = player2 + QString(">board>");
            //sendCommand(prepend + b.printPublicBoard().c_str() + ">");
        }*/
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
            queueCommand(PRINT, QString("That move has already been attempted. Try again. \n"));
            //sendCommand(QString("That move has already been attempted. Try again. \n"));
            //std::cout<<"That move has already been attempted. Try again. \n";

        queueCommand(PRINT, QString("Please enter location [Letter][Number] of desired move:\n"));
        //sendCommand(QString("Please enter location [Letter][Number] of desired move:\n"));
        //std::cout<<"Please enter location [Letter][Number] of desired move:\n";
        entryTemp=getSquare();
        xEntry=static_cast<int>(entryTemp[0]);
        yEntry=static_cast<int>(entryTemp[1]);

        if (b.getSpaceValue(xEntry-LETTER_CHAR_OFFSET,
                        yEntry-NUMBER_CHAR_OFFSET)!=isHIT
            && b.getSpaceValue(xEntry-LETTER_CHAR_OFFSET,
                            yEntry-NUMBER_CHAR_OFFSET)!=isMISS)
        {
            b.recordHit(xEntry-LETTER_CHAR_OFFSET, yEntry-NUMBER_CHAR_OFFSET);
            goodMove=true;

            /*
            QString returnval;
            b.recordHit(xEntry-LETTER_CHAR_OFFSET, yEntry-NUMBER_CHAR_OFFSET, &returnval);
            if(returnval != QString("none")){
               sendCommand(returnval);
            }
            goodMove=true;
            */
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

        if (b.getSpaceValue(xEntry, yEntry)!=isHIT
            && b.getSpaceValue(xEntry, yEntry)!=isMISS)
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
    retString = receiveCommand().toStdString();
    //retString = receiveCommand(activeplayer, NULL).toStdString();
    //std::getline(std::cin, retString);
    bool isGoodInput=false;

    while (!isGoodInput)
    {
        // check for two character entries of letter/number
        if (retString.length()==2 && (retString[0]>=65 && retString[0]<=74)
                        && (retString[1]>=48 && retString[1]<=57))
            isGoodInput=true;
        else
        {
            queueCommand(PRINT, QString("Bad input! Please enter location [Letter][Number] of your desired move, with capital letters only:\n"));
            //sendCommand("Bad input! Please enter location [Letter][Number] of your desired move, with capital letters only:\n");
            //std::cout<<"Bad input! Please enter location [Letter][Number] of "
                        //<<"your desired move, with capital letters only:\n";
            queueCommand(REQUEST, QString("string"));
            sendCommand(activeplayer);
            retString = receiveCommand().toStdString();
            //retString = receiveCommand(activeplayer, NULL).toStdString();
            //std::getline(std::cin, retString);
        }
    }

    return retString;
}


// switchPlayers is a function that controls the screen between turns,
// ensuring that the player whose turn it is can control what is visible
// on the screen in case someone else is peeking
/*
void Service::switchPlayers(std::string playerFrom, std::string playerTo)
{
    sendCommand(QString(playerFrom.c_str()) + QString(", press ENTER to finish your turn!"));
    // std::cout<<playerFrom<<", press ENTER to finish your turn!";
    std::cin.get();
    std::cout<<std::flush;
    //sendCommand(QString(""));
    std::cout<<std::string(100,'\n');
    sendCommand(QString(playerTo.c_str()) + QString(", press ENTER to start your turn!"));
    //std::cout<<playerTo<<", press ENTER to start your turn!";
    std::cin.get();
    std::cout<<std::flush;
    //sendCommand(QString(""));
    std::cout<<std::string(100,'\n');
}
*/
