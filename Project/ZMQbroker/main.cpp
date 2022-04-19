#include <iostream>
#include <zmq.hpp>

int main( void )
{
	try
	{
		zmq::context_t context(1);

		//Incoming messages come in here
		zmq::socket_t sink( context, ZMQ_PULL );
		sink.bind( "tcp://*:24041" );

		//Outgoing message go out through here
		zmq::socket_t publisher( context, ZMQ_PUB );
		publisher.bind( "tcp://*:24042" );

		//Linking the one socket to the other
		zmq::proxy( sink, publisher, nullptr );
	}
	catch( zmq::error_t & ex )
	{
		std::cerr << "Caught an exception : " << ex.what();
	}

	return 0;
}
