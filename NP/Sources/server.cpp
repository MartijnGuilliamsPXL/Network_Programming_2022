#include "server.h"

Server::Server(QObject *parent):
    QObject(parent)
{
    //default constructor, ignore
}

Server::Server(QObject *parent, QString ServerString):
    QObject(parent),
    ServerString(ServerString)
{
    fill_n(lobbies, size(lobbies), false);
    QObject::connect(this, &Server::requestLobby, this, &Server::addLobby);
    /*
    //prepare sender
    sender = new zmq::socket_t(*context_p, ZMQ_PUSH);
    sender->connect("tcp://benternet.pxl-ea-ict.be:24041");*/

    /*
    player1(userid);
    player2("");
    isStarted(false);
    player2Auto(false);
    standardPrefix("Zeeslag>");

    //Zeeslag>list>userID>

    //Zeeslag>userID>list>gameID>CPU>gameID>PVP>

    //prepare acknowledgement before new standardprefix
    QString ack = standardPrefix + player1 + '>' + QString(to_string(lobbynr).c_str()) + '>';

    //update prefix to standard
    standardPrefix.append(to_string(lobbynr).c_str());
    standardPrefix.append('>');
    //make sub string
    QString substring = standardPrefix + QString("user>");
    standardPrefix.append("server>");
    */


    //socket setup
    context_p = new zmq::context_t(1);
    receiver = new zmq::socket_t( *context_p, ZMQ_SUB);
    sender = new zmq::socket_t(*context_p, ZMQ_PUSH);
    //make connection
    sender->connect("tcp://benternet.pxl-ea-ict.be:24041");

    QString Topic = ServerString + "create>";
    receiver->connect("tcp://benternet.pxl-ea-ict.be:24042");
    receiver->setsockopt(ZMQ_SUBSCRIBE, Topic.toStdString().c_str(), Topic.toStdString().size());

    QString Topic2 = ServerString + "list>";
    //receiver->connect("tcp://benternet.pxl-ea-ict.be:24042");
    receiver->setsockopt(ZMQ_SUBSCRIBE, Topic2.toStdString().c_str(), Topic2.toStdString().size());

}


void Server::runServer()
{
    //receiveLoop();
    QtConcurrent::run(&Server::receiveLoop, this);
}

void Server::killService(Service *toKill)
{
    int lobby = toKill->getLobby();
    lobbies[lobby] = false;
    delete toKill;
    cout << "killed service with lobbynr: " << lobby << endl;
}


void Server::addLobby(QString id)
{
    int lobbynr = requestLobbyNr();

    Service* newService = new Service(this, id, lobbynr);
    QObject::connect(newService, &Service::killme, this, &Server::killService);
    services.push_back(newService);
    cout << "created lobby with nr: " << lobbynr << endl;
    newService->run();

}

void Server::receiveLoop()
{

    cout << "starting receiveloop" << endl;
    while(1){
        //receive file
        zmq::message_t* datapayload = new zmq::message_t;
        receiver->recv(datapayload);
        QString fullCommand = QString(string((char*)(datapayload->data()), datapayload->size()).c_str());
        //fullCommand = "Zeeslag>create>456>";
        cout << fullCommand.toStdString() << endl;
        if(fullCommand.section('>', 1, 1) == QString("create")){
            emit requestLobby(fullCommand.section('>', 2, 2));
            cout << "testing: if" << endl;
        }

        else if(fullCommand.section('>', 1, 1) == QString("list"))
        {
            cout << "testing: else if" << endl;


            int lobbynr = requestLobbyNr();
            lobbies[lobbynr] = false;
            QString lobbyRequest = "Zeeslag>userID>list>gameID>" +  QString::number(lobbynr) + '>';




            //QString lobbyRequest = standardPrefix + "create>" + userID + '>'
            sender->send(lobbyRequest.toStdString().c_str(), lobbyRequest.size());



            //if(fullCommand.section('>', 1, 1) == QString("info"))
            //requestLobbyNr();
            //Zeeslag>list>userID>
            //Zeeslag>userID>list>gameID>CPU>gameID>PVP>
            //QString tosend = standardPrefix + player1 + '>'  + queuedCommand;

            //QString tosend = "Zeeslag>userID>list>gameID>CPU>gameID>PVP>";
            //sender->send(tosend.toStdString().c_str(), tosend.size());
            //emit requestLobby(fullCommand.section('>', 2, 2));

            //[Zeeslag>create>9657>]
            //[Zeeslag>list>9550>]
        }
    }
}

int Server::requestLobbyNr()
{
    for(int i = 0; i < 100; i++){
        if(!lobbies[i]){
            lobbies[i] = true;
            return i;
        }
    }
    return -1;
}
