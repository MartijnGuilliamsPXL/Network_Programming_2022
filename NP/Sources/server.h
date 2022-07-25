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
public:

    explicit Server(QObject *parent = nullptr);
    Server(QObject* parent, QString ServerString);
    void runServer();

signals:
    void requestLobby(QString id);

private:

    void receiveLoop();
    int requestLobbyNr();

    QString ServerString;
    vector<Service*> services;
    vector<bool> lobbies;

public slots:
    void addLobby(QString id);
};

#endif // SERVER_H
