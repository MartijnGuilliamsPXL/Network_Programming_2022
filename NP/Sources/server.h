#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QString>
#include <vector>
#include <service.h>
#include "zmq.hpp"
#include <iostream>
#include <QtConcurrent>
#include <QFuture>
#include <QThread>

using namespace std;

class Server : public QObject
{
    Q_OBJECT

signals:
    void requestLobby(QString id);

public:

    explicit Server(QObject *parent = nullptr);
    Server(QObject* parent, QString ServerString);
    void runServer();

public slots:



private:

    ///////
    /// commands
    /// naam>servicenaam>create>USRID
    ///

    void receiveLoop();
    int requestLobbyNr();

    zmq::context_t* context_p;
    zmq::socket_t* sender;
    zmq::socket_t* receiver;

    QString ServerString;
    vector<Service*> services;
    bool lobbies[100];

private slots:

    void addLobby(QString id);
    void killService(Service* toKill);
};

#endif // SERVER_H
