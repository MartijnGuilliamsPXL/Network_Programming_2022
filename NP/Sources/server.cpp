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
    fill(lobbies.begin(), lobbies.end(), false);
    QObject::connect(this, &Server::requestLobby, this, &Server::addLobby);
}


void Server::runServer()
{
    QtConcurrent::run(&Server::receiveLoop, this);
}



void Server::addLobby(QString id)
{
    int lobbynr = requestLobbyNr();
    Service* newService = new Service(this, id, lobbynr);
    services.push_back(newService);
    cout << "created lobby with nr: " << lobbynr << endl;
    newService->run();

}

void Server::receiveLoop()
{
    //socket setup
    zmq::context_t context(1);
    zmq::socket_t receiver( context, ZMQ_SUB);
    //make connection
    receiver.connect("tcp://benternet.pxl-ea-ict.be:24042");
    receiver.setsockopt(ZMQ_SUBSCRIBE, ServerString.toStdString().c_str(), ServerString.size());
    cout << "starting receiveloop" << endl;
    while(1){
        //receive file
        zmq::message_t* datapayload = new zmq::message_t;
        receiver.recv(datapayload);
        QString fullCommand = QString((char*)(datapayload->data()));
        //fullCommand = "name>service>456>create>";
        cout << fullCommand.toStdString() << endl;
        emit requestLobby(fullCommand.section('>', 3, 3));
        break;
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
