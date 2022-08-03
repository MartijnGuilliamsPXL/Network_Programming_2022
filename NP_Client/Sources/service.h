#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QString>
#include <vector>
#include <zmq.hpp>
#include <QtConcurrent>
#include <iostream>



#include <cstdlib>
#include <ctime>




using namespace std;

class Service : public QObject
{
    Q_OBJECT
public:
    explicit Service(QObject *parent = nullptr);

    Service(QObject *parent, QString userid, int lobbynr);
    ~Service();
    void run();
    int getLobby();

signals:
    void killme(Service* toKill);
private:
    ////////////////
    /// //in
    /// naam>servicenaam>lobby>user>USRID>command>val>...
    /// //out
    /// naam>servicenaam>lobby>server>player>command>val...
    ///
    /// player USRID/ 0 => allebei

    int lobby;
    zmq::socket_t* sender;
    zmq::socket_t* receiver;
    zmq::context_t* context_p;

    QString standardPrefix;

    void Loop();
    int processCommand(QString command);
    void sendCommand(QString command);
};

#endif // SERVICE_H
