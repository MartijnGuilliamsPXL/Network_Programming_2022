#include "service.h"

Service::Service(QObject *parent) :
    QObject(parent)
{
    //default constructor, ignore
}

Service::Service(QObject *parent, QString userid, int lobbynr):
    QObject(parent),

    standardPrefix("name>servicename>")
{
    context_p = new zmq::context_t(1);

    //prepare sender
    sender = new zmq::socket_t(*context_p, ZMQ_PUSH);
    sender->connect("tcp://benternet.pxl-ea-ict.be:24041");



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


}

Service::~Service()
{
    delete sender;
    delete receiver;
    delete context_p;
}

void Service::run()
{
    QtConcurrent::run(&Service::Loop, this);
    //loop();
}

void Service::Loop()
{
    zmq::message_t* datapayload;
    while(1){
        datapayload = new zmq::message_t;
        receiver->recv(datapayload);
        QString command = QString(string((char*) datapayload->data(), datapayload->size()).c_str());
        delete datapayload;
        cout << command.toStdString() << endl;
        if(processCommand(command.section('>', 4))){
            break;
        }
    }
    emit killme(this);
}

int Service::processCommand(QString command)
{
    //a lot of if statement checking the command
    //try to make player 2 auto;

    //exit and delete lobby
    if(command.section('>', 1, 1) == QString("exit")){
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


