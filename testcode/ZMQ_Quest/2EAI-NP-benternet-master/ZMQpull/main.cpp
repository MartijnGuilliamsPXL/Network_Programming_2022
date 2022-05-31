#include <iostream>
#include <string>
#include <zmq.hpp>

int main( void )
{
	try
	{
		zmq::context_t context(1);

		//Incoming messages come in here
		zmq::socket_t sink( context, ZMQ_PULL );
		sink.bind( "tcp://*:24041" );

		zmq::message_t * msg = new zmq::message_t();
		while( sink.connected() )
		{
			sink.recv( msg );
			std::cout << "Pulled : [" << std::string( (char*) msg->data(), msg->size() ) << "]" << std::endl;
		}
	}
	catch( zmq::error_t & ex )
	{
		std::cerr << "Caught an exception : " << ex.what();
	}

	return 0;
}
