#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QString>
#include <vector>
#include <zmq.hpp>
#include <QtConcurrent>
#include <iostream>

#include <stdlib.h>
#include <time.h>


enum commands {REQUEST, PRINT, RESPONSE};

using namespace std;

class Service : public QObject
{
    Q_OBJECT
public:
    //explicit Service(QObject *parent = nullptr);

    Service();
    ~Service();
    void run();
    int getLobby();

signals:
    void killme(Service* toKill);
private:
    ////////////////
    /// //in
    /// Zeeslag>lobby>user>USRID>command>val>...
    /// //out
    /// Zeeslag>lobby>server>player>command>val...
    ///
    /// player USRID/ 0 => allebei


    QString userID;
    QString lobby;
    zmq::socket_t* sender;
    zmq::socket_t* receiver;
    zmq::context_t* context_p;

    QString standardPrefix;

    void list();
    void setupLobby();
    void joinLobby();

    void Loop();
    int processCommand(QString command);
    void sendCommand(int command, QString value);
    QString receiveString();
};

#endif // SERVICE_H
