#include <iostream>
#include <zmq.hpp>
#ifndef _WIN32
	#include <unistd.h>
#else
	#include <windows.h>
	#define sleep(n)    Sleep(n)
#endif

int main( void )
{
	try
	{
        zmq::context_t context(2);

		//Outgoing message go out through here
		zmq::socket_t ventilator( context, ZMQ_PUSH );
        //ventilator.connect( "tcp://localhost:24041" );
        ventilator.connect( "tcp://benternet.pxl-ea-ict.be:24041" );

        zmq::message_t * msg = new zmq::message_t();
		while( ventilator.connected() )
		{
			sleep( 1000 );
            ventilator.send( "Zeeslag>", 8 );
            std::cout << "Pushed : [ping]" << std::endl;
            /*while( ventilator.connected() )
            {
                ventilator.recv( msg );
                std::cout << std::string( (char*) msg->data()) << std::endl;
            }*/
		}
	}
	catch( zmq::error_t & ex )
    {
		std::cerr << "Caught an exception : " << ex.what();
	}

	return 0;
}
