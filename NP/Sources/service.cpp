#include "service.h"

Service::Service(QObject *parent) :
    QObject(parent)
{
    //default constructor, ignore
}

Service::Service(QObject *parent, QString userid, int lobbynr):
    QObject(parent),
    lobby(lobbynr),
    standardPrefix("naam>servicenaam>")
{
    zmq::context_t context(0);
    //prepare sender
    sender = new zmq::socket_t(context, ZMQ_PUSH);
    sender->connect("tcp://benternet.pxl-ea-ict.be:24041");
    //prepare acknowledgement
    QString ack = standardPrefix + userid + '>' + QString(to_string(lobbynr).c_str()) + '>';
    //update prefix to standard
    standardPrefix.append(to_string(lobbynr).c_str());
    standardPrefix.append('>');
    //prepare receiver
    receiver = new zmq::socket_t(context, ZMQ_SUB);
    receiver->connect("tcp://benternet.pxl-ea-ict.be:24042");
    receiver->setsockopt(ZMQ_SUBSCRIBE, standardPrefix.toStdString().c_str(), standardPrefix.size());
    //send the acknowledgement, the lobby is prepared
    sender->send(ack.toStdString().c_str(), ack.size());
}

void Service::run()
{
    QtConcurrent::run(&Service::loop, this);
}

void Service::loop()
{
    zmq::message_t* datapayload;
    while(1){
        datapayload = new zmq::message_t;
        receiver->recv(datapayload);
        QString command = QString(string((char*) datapayload->data(), datapayload->size()).c_str());
        delete datapayload;
        if(processCommand(command.section('>', 4))){
            break;
        }
    }
}

int Service::processCommand(QString command)
{
    //a lot of if statement checking the command
    if(command.section('>', 0, 0) == QString("exit")){
        return 1;
    }
    return 0;
}

void Service::sendCommand(QString command)
{
    sender->send(command.toStdString().c_str(), command.size());
}
