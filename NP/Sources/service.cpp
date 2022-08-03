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
}

Service::~Service()
{
    delete sender;
    delete receiver;
    delete context_p;
}

void Service::run()
{
    QtConcurrent::run(&Service::setupLoop, this);
    //loop();
}

int Service::getLobby()
{
    return lobby;
}

void Service::setupLoop()
{
    zmq::message_t* datapayload;
    while(1){
        datapayload = new zmq::message_t;
        receiver->recv(datapayload);
        QString command = QString(string((char*) datapayload->data(), datapayload->size()).c_str());
        delete datapayload;
        sendCommand(QString(command.toStdString().c_str()));
        //cout << command.toStdString() << endl;
        if(processSetupCommand(command.section('>', 4))){
            break;
        }
    }
    emit killme(this);
}

int Service::processSetupCommand(QString command)
{
    //a lot of if statement checking the command
    //try to make player 2 auto;
    if(command.section('>', 1, 1) == QString("autojoin")){
        if(!isStarted){
            player2 = "CPU";
            player2Auto = true;
            isStarted = true;
            sendCommand(QString("lobby ") + QString(char(lobby)) + QString(": player two is CPU"));
            //cout << "lobby " << lobby << ": player two is CPU" << endl;
            playGame();
        }
    }
    //try to add player 2
    else if(command.section('>', 1, 1) == QString("join")){
        if(!isStarted){
            if(player1 == command.section('>', 0, 0)){
                sendCommand(QString("err>P2connotBeP1>"));
            }
            else{
                player2 = command.section('>', 0, 0);
                isStarted = true;
                sendCommand(QString("lobby ") + QString(char(lobby)) + QString(": player two joined, id: ") + QString(player2.toStdString().c_str()));
                //cout << "lobby " << lobby << ": player two joined, id: " << player2.toStdString() << endl;
                playGame();
            }
        }
        else{
            sendCommand(QString("err>lobbyfull>"));
        }
    }
    //exit and delete lobby
    else if(command.section('>', 1, 1) == QString("exit")){
        sendCommand(QString(">print>exiting lobby, thanks for playing>"));
        cout << "exit lobby: " << lobby << endl;
        return 1;
    }
    else{
        sendCommand(QString("err>unknown command>"));
    }
    return 0;
}

void Service::sendCommand(QString command)
{
    QString payload = standardPrefix + command;
    sender->send(payload.toStdString().c_str(), payload.size());
}

QString Service::receiveCommand(int player, QString desiredplayercommand)
{
    zmq::message_t* datapayload;
    while(1){
        datapayload = new zmq::message_t;
        receiver->recv(datapayload);
        QString command = QString(string((char*) datapayload->data(), datapayload->size()).c_str());
        delete datapayload;
        //if the player and the command is correcct
        if((command.section('>', 4, 4) == player1 && player == 1 || command.section('>', 4, 4) == player2 && player == 2 || player == 0) && command.section('>', 5, 5) == desiredplayercommand){
            return command.section('>', 6);
        }
    }
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
            printGameState(*(pptr));
        }

        // switch pointers at the end of each turn
        if ((*pptr).getPlayerNum()==1)
        {
            if (!p1.isPlayerAutomatic() && !p2.isPlayerAutomatic())
            {
                //switchPlayers(p1.getName(), p2.getName());
            }
            pptr = &p2;
            bptr = &p1Board;
        }
        else {
            if (!p1.isPlayerAutomatic() && !p2.isPlayerAutomatic())
            {
                //switchPlayers(p2.getName(), p1.getName());
            }
            pptr = &p1;
            bptr = &p2Board;
        }
    }

    if (gameCondition()==P1_WIN)
        sendCommand(QString(p1.getName().c_str() + QString(" wins!!!")));
        //std::cout<<p1.getName()<<" wins!!!"<<std::endl;
    else
        sendCommand(QString(p2.getName().c_str() + QString(" wins!!!")));
        //std::cout<<p2.getName()<<" wins!!!"<<std::endl;

    sendCommand(QString("I hope you enjoyed the Battleship game. Bye!\n\n"));
    //std::cout<<"I hope you enjoyed the Battleship game. Bye!\n\n";
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
    activeplayer = 1;
    sendCommand(player1 + QString(">q>binval>print>how would you like your board to be set? (enter 0 for non-auto, 1 for auto)\n"));
    autoTemp = receiveCommand(1, QString("binval")).toInt();
    if (autoTemp)
        initializeBoardAuto(p1Board, true);
    else initializeBoard(p1Board);


    // if both players are non-auto, clear screen appropriately between fills
    if (!p2.isPlayerAutomatic() && !p1.isPlayerAutomatic())
    {
        //switchPlayers(p1.getName(), p2.getName());
    }

    if (p2.isPlayerAutomatic())
        initializeBoardAuto(p2Board, false);
    else
    {

        sendCommand(QString("player 2>how would you like your board to be set? (enter 0 for non-auto, 1 for auto)\n"));
        autoTemp = receiveCommand(2, QString("binval")).toInt();
        if (autoTemp)
            initializeBoardAuto(p2Board, true);
        else initializeBoard(p2Board);

    }
    return;
}

// prints game state, with player's own board exposed and opponent's board
// mostly obscured, save for moves player has made
void Service::printGameState(Player p)
{
    sendCommand(QString(p.getName().c_str()) + QString("'s GAME STATE:\n\n"));
    //std::cout<<p.getName()<<"'s GAME STATE:\n\n";
    if (p.getPlayerNum()==1)
    {
        sendCommand(QString("YOUR BOARD: \n"));
        //std::cout<<"YOUR BOARD: \n";
        sendCommand(QString(p1Board.printPublicBoard().c_str()));
        //p1Board.printPublicBoard();
        sendCommand(QString("YOUR OPPONENT'S BOARD: \n"));
        //std::cout<<"YOUR OPPONENT'S BOARD: \n";
        sendCommand(QString(p2Board.printPrivateBoard().c_str()));
        //p2Board.printPrivateBoard();
        //std::cout<<std::endl<<std::endl;
    }
    else
    {
        sendCommand(QString("YOUR BOARD: \n"));
        //std::cout<<"YOUR BOARD: \n";
        sendCommand(QString(p2Board.printPublicBoard().c_str()));
        //p2Board.printPublicBoard();
        sendCommand(QString("YOUR OPPONENT'S BOARD: \n"));
        //std::cout<<"YOUR OPPONENT'S BOARD: \n";
        sendCommand(QString(p1Board.printPrivateBoard().c_str()));
        //p1Board.printPrivateBoard();
        //std::cout<<std::endl<<std::endl;
    }


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
            sendCommand(QString("INVALID ENTRY for that ship! Please try again. \n"));
            //std::cout<<"INVALID ENTRY for that ship! Please try again. \n";

            sendCommand(QString("Please enter location [Letter][Number] for the top/left of your " + QString(SHIP_NAMES[i].c_str()) + " which is length " + QString(char(SHIP_LENGTHS[i])) + ": \n"));
            //std::cout<<"Please enter location [Letter][Number] for the "<<
            //            "top/left of your "<<SHIP_NAMES[i]<<" which is length "
            //            <<SHIP_LENGTHS[i]<<": \n";
            entryTemp=getSquare();
            xEntry=static_cast<int>(entryTemp[0]);
            yEntry=static_cast<int>(entryTemp[1]);

            sendCommand(QString("Please enter 0 if the ship is oriented vertically, 1 if it is oriented horizontally:\n"));
            //std::cout<<"Please enter 0 if the ship is oriented vertically, "
            //            <<"1 if it is oriented horizontally:\n";

            sendCommand(QString("prestuff>print>geef 0 als het schip verticaal moet of 1 voor horizontaal>binval>req>"));
            horizEntry = receiveCommand(1, QString("binval")).toInt();
            // NAKIJKEN !!!!!!!!!!!!
            //horizEntry=getInt(0,1);

            attemptCount++;
        } while (!b.placeShip(i, xEntry-LETTER_CHAR_OFFSET,
                        yEntry-NUMBER_CHAR_OFFSET, horizEntry));

    }

    sendCommand(QString("Your starting board: \n"));
    //std::cout<<"Your starting board: \n";
    sendCommand(QString(b.printPublicBoard().c_str()));
    //b.printPublicBoard();

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
        sendCommand(QString("Your starting board: \n"));
        //std::cout<<"Your starting board: \n";
        if(activeplayer == 1){
            QString prepend = player1 + QString(">board>");
            sendCommand(prepend + b.printPublicBoard().c_str() + ">");
        }
        else{
            QString prepend = player2 + QString(">board>");
            sendCommand(prepend + b.printPublicBoard().c_str() + ">");
        }
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
            sendCommand(QString("That move has already been attempted. Try again. \n"));
            //std::cout<<"That move has already been attempted. Try again. \n";

        sendCommand(QString("Please enter location [Letter][Number] of desired move:\n"));
        //std::cout<<"Please enter location [Letter][Number] of desired move:\n";
        entryTemp=getSquare();
        xEntry=static_cast<int>(entryTemp[0]);
        yEntry=static_cast<int>(entryTemp[1]);

        if (b.getSpaceValue(xEntry-LETTER_CHAR_OFFSET,
                        yEntry-NUMBER_CHAR_OFFSET)!=isHIT
            && b.getSpaceValue(xEntry-LETTER_CHAR_OFFSET,
                            yEntry-NUMBER_CHAR_OFFSET)!=isMISS)
        {
            QString returnval;
            b.recordHit(xEntry-LETTER_CHAR_OFFSET, yEntry-NUMBER_CHAR_OFFSET, &returnval);
            if(returnval != QString("none")){
                sendCommand(returnval);
            }
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

        if (b.getSpaceValue(xEntry, yEntry)!=isHIT
            && b.getSpaceValue(xEntry, yEntry)!=isMISS)
        {
            QString returnval;
            b.recordHit(xEntry, yEntry, &returnval);
            if(returnval != QString("none")){
                sendCommand(returnval);
            }
        }
    }
    return;
}

// input validation for square
std::string Service::getSquare()
{
    std::string retString;
    retString = receiveCommand(activeplayer, NULL).toStdString();
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
            sendCommand("Bad input! Please enter location [Letter][Number] of your desired move, with capital letters only:\n");
            //std::cout<<"Bad input! Please enter location [Letter][Number] of "
                        //<<"your desired move, with capital letters only:\n";
            retString = receiveCommand(activeplayer, NULL).toStdString();
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

