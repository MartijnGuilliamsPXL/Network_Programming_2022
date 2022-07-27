#include <iostream>
#include <vector>
#include <zmq.hpp>


using namespace std;


int main()
{
    int tochoose = 0;
    string prepend = "";
    cout << "choose operation:" << endl << "0 = normal" << endl << "1 = zeeslag service" << endl;
    cin >> tochoose;
    switch (tochoose) {
    case 0:
        break;
    case 1:
        prepend = "name>servicename>";
    }
    //create socket
    zmq::context_t context(1);
    zmq::socket_t* pusher;
    pusher = new zmq::socket_t( context, ZMQ_PUSH);
    pusher->connect("tcp://benternet.pxl-ea-ict.be:24041");
    string message;
    while(1){
        cin >> message;
        message = prepend + message;
        cout << "sending message: " << message << endl;
        pusher->send(message.c_str(), message.length());
    }
    return 0;
}








