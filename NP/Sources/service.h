#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>
#include <QString>
#include <vector>
#include <zmq.hpp>
#include <QtConcurrent>

using namespace std;

class Service : public QObject
{
    Q_OBJECT
public:
    explicit Service(QObject *parent = nullptr);

    Service(QObject *parent, QString userid, int lobbynr);
    void run();

signals:

private:
    int lobby;
    zmq::socket_t* sender;
    zmq::socket_t* receiver;
    QString standardPrefix;

    void loop();
    int processCommand(QString command);
    void sendCommand(QString command);
    // Game logic


};

#endif // SERVICE_H
