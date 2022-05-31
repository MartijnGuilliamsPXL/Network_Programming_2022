#include <iostream>
#include <string>
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
		zmq::context_t context(1);

		//Outgoing message go out through here
		zmq::socket_t client( context, ZMQ_REQ );
		client.connect( "tcp://localhost:24040" );
//		client.connect( "tcp://benternet.pxl-ea-ict.be:24040" );

		zmq::message_t * msg = new zmq::message_t();
		while( client.connected() )
		{
			sleep( 1000 );
			client.send( "ping", 4 );
			std::cout << "Requested : [ping]" << std::endl;
			client.recv( msg );
			std::cout << "Received : [" << std::string( (char*) msg->data(), msg->size() ) << "]" << std::endl;
		}
	}
	catch( zmq::error_t & ex )
	{
		std::cerr << "Caught an exception : " << ex.what();
	}

	return 0;
}
