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
    /// create lobby: Zeeslag>create>userID>           ///
    /// lobby created: Zeeslag>userID>lobby>           ///
    ///                                                         ///
    /// general commands: Zeeslag>lobby>userID>command ///
    ///                                                         ///
    ///////////////////////////////////////////////////////////////



    Server nr1(nullptr, "Zeeslag>");
    nr1.runServer();
    cout << "hello world" << endl;
    return a.exec();
}
