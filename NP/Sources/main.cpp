#include <QCoreApplication>
#include <QThread>
#include <iostream>
#include "server.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ///////////////////////////////////////////////////////////////
    /// commands                                                ///
    /// create lobby: name>servicename>create>userID>           ///
    /// lobby created: name>servicename>userID>lobby>           ///
    ///                                                         ///
    /// general commands: name>servicename>lobby>userID>command ///
    ///                                                         ///
    ///////////////////////////////////////////////////////////////



    Server nr1(nullptr, "name>servicename>");
    nr1.runServer();
    cout << "hello world" << endl;
    return a.exec();
}
