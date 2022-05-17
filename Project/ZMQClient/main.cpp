#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;


int main (void)
{
    printf ("Connecting to hello world server...\n");
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");

    while (true) {

        string input = "";
        cout << "Doe ma: ";
        cin >> input;

        char buffer [30];
        printf ("Sending\n");

        const void * a = input.c_str();
        zmq_send (requester, a, 21, 0);
        zmq_recv (requester, buffer, 21, 0);

        cout << "Received: " << buffer << "\n";
        sleep(1);
    }

    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;
}
