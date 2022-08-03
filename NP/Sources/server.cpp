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

    //socket setup
    context_p = new zmq::context_t(1);
    receiver = new zmq::socket_t( *context_p, ZMQ_SUB);
    sender = new zmq::socket_t(*context_p, ZMQ_PUSH);
    //make connection
    QString Topic = ServerString + "create>";
    receiver->connect("tcp://benternet.pxl-ea-ict.be:24042");
    receiver->setsockopt(ZMQ_SUBSCRIBE, Topic.toStdString().c_str(), Topic.toStdString().size());

}


void Server::runServer()
{
    QtConcurrent::run(&Server::receiveLoop, this);
    //receiveLoop();
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
        //fullCommand = "name>servicename>create>456>";
        cout << fullCommand.toStdString() << endl;
        if(fullCommand.section('>', 2, 2) == QString("create")){
            emit requestLobby(fullCommand.section('>', 3, 3));
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
