#include "service.h"


/*Service::Service(QObject *parent) :
    QObject(parent)
{
    //default constructor, ignore
}*/

Service::Service():
    QObject(nullptr),

    standardPrefix("name>servicename>")
{
    //prepare userid
    srand(time(NULL));
    userID = QString(to_string((rand() % 32767)).c_str());
    cout << "user id is: " << userID.toStdString() << endl;
    //prepare network comms
    context_p = new zmq::context_t(1);

    //prepare sender
    sender = new zmq::socket_t(*context_p, ZMQ_PUSH);
    sender->connect("tcp://benternet.pxl-ea-ict.be:24041");

    //prepare receiver
    receiver = new zmq::socket_t(*context_p, ZMQ_SUB);
    receiver->connect("tcp://benternet.pxl-ea-ict.be:24042");
}

Service::~Service()
{
    delete sender;
    delete receiver;
    delete context_p;
}

void Service::run()
{
    while(1){
        string choice;
        cout << "do you want to create a lobby or join one? (c/j)" << endl;
        cin >> choice;
        QString Qchoice = choice.c_str();
        if(Qchoice == QString("c")){
            setupLobby();
            break;
        }
        else if(Qchoice == QString("j")){
            joinLobby();
            break;
        }
        cout << "error, try again" << endl;
    }

    //no need for concurrency
    //QtConcurrent::run(&Service::Loop, this);
    Loop();
}

void Service::setupLobby(){
    //prepare for lobby response
    QString SubscribeString1 = standardPrefix + userID + '>';
    receiver->setsockopt(ZMQ_SUBSCRIBE, SubscribeString1.toStdString().c_str(), SubscribeString1.size());
    //send lobby request
    QString lobbyRequest = standardPrefix + "create>" + userID + '>';
    sender->send(lobbyRequest.toStdString().c_str(), lobbyRequest.size());
    //receive lobby
    QString lobby = receiveString().section('>', 3, 3);
    cout << "received lobby number " << lobby.toStdString().c_str() << endl;
    //update to lobby comms
    QString SubscribeString2 = standardPrefix + lobby + '>' + "server>" +  userID + '>';
    cout << "subscribed to " << SubscribeString2.toStdString() << endl;
    receiver->setsockopt(ZMQ_SUBSCRIBE, SubscribeString2.toStdString().c_str(), SubscribeString2.size());
    standardPrefix = standardPrefix + lobby + ">user>" +userID + '>';
    sendCommand(RESPONSE, QString("ready"));
}

void Service::joinLobby(){
    string lobbyreq;
    cout << "give the lobbynumber of the one you want to join" << endl;
    cin >> lobbyreq;
    lobby = QString(lobbyreq.c_str());
    QString SubscribeString2 = standardPrefix + lobby + '>' + "server>" +  userID + '>';
    cout << "subscribed to " << SubscribeString2.toStdString() << endl;
    receiver->setsockopt(ZMQ_SUBSCRIBE, SubscribeString2.toStdString().c_str(), SubscribeString2.size());
    standardPrefix = standardPrefix + lobby + ">user>" +userID + '>';
    sendCommand(RESPONSE, QString("join"));

}

void Service::Loop()
{
    zmq::message_t* datapayload;
    while(1){
        datapayload = new zmq::message_t;
        receiver->recv(datapayload);
        QString command = QString(string((char*) datapayload->data(), datapayload->size()).c_str());
        delete datapayload;
        //cout << command.toStdString() << endl;
        if(processCommand(command.section('>', 5))){
            break;
        }
    }
    emit killme(this);
}

int Service::processCommand(QString command)
{
    //get the amount of commands in the string
    int loops = command.split('>').size();
    //cout << "this string has " << loops / 2  << " commands" << endl;
    for(int i = 0; i < loops; i += 2){
        //request
        if(command.section('>', i, i) == QString("request")){
            string request;
            //get input and check for correct value
            while(1){
                cin >> request;
                QString Qrequest = request.c_str();
                //check type
                if(command.section('>', i + 1, i + 1) == QString("bool")){
                    if(Qrequest == QString("0") || Qrequest == QString("1") || Qrequest == QString("y") || Qrequest == QString("n") ){
                        if(Qrequest == QString("y")){
                            request = "1";
                        }
                        else if(Qrequest == QString("n")){
                            request = "0";
                        }
                        break;
                    }
                }
                else if(command.section('>', i + 1, i + 1) == QString("string")){
                    break;
                }
                else{
                    cout << "error bad request" << endl;
                }
                cout << "wrong input for type " << command.section('>', i + 1, i + 1).toStdString() << ", please try again" << endl;
            }
            sendCommand(RESPONSE, QString(request.c_str()));
        }
        //print
        else if(command.section('>', i, i) == QString("print")){
            cout << command.section('>', i + 1, i + 1).toStdString() << endl;
        }
        //exit and delete lobby
        else if(command.section('>', i, i) == QString("exit")){
            return 1;
        }
    }
    return 0;
}

void Service::sendCommand(int command, QString value)
{
    QString tosend = standardPrefix;
    switch (command){
    case REQUEST:
        tosend = tosend + "request>" + value + '>';
        break;
    case PRINT:
        tosend = tosend + "print>" + value + '>';
        break;
    case RESPONSE:
        tosend = tosend + "response>" + value + '>';
        break;
    }

    sender->send(tosend.toStdString().c_str(), tosend.size());
}

QString Service::receiveString()
{
    zmq::message_t* datapayload;
    datapayload = new zmq::message_t;
    receiver->recv(datapayload);
    QString message = QString(string((char*) datapayload->data(), datapayload->size()).c_str());
    delete datapayload;
    //cout << message.toStdString() << endl;
    return message;
}


