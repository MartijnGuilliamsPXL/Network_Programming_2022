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
		zmq::context_t context(1);

		//Outgoing message go out through here
		zmq::socket_t publisher( context, ZMQ_PUB );
		publisher.bind( "tcp://*:24042" );

		while( publisher.connected() )
		{
			sleep( 1000 );
			publisher.send( "ping", 4 );
			std::cout << "Published : [ping]" << std::endl;
		}
	}
	catch( zmq::error_t & ex )
	{
		std::cerr << "Caught an exception : " << ex.what();
	}

	return 0;
}
