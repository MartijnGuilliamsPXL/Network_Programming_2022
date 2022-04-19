#include <iostream>
#include <string>
#include <zmq.hpp>

int main( void )
{
	try
	{
		zmq::context_t context(1);

		//Outgoing message go out through here
		zmq::socket_t server( context, ZMQ_REP );
		server.bind( "tcp://*:24040" );

		zmq::message_t * msg = new zmq::message_t();
		while( server.connected() )
		{
			server.recv( msg );
			std::string s( (char*) msg->data(), msg->size() );
			std::cout << "Received : [" << s << "]" << std::endl;
			server.send( *msg );
			std::cout << "Replied : [" << s << "]" << std::endl;
		}
	}
	catch( zmq::error_t & ex )
	{
		std::cerr << "Caught an exception : " << ex.what();
	}

	return 0;
}
