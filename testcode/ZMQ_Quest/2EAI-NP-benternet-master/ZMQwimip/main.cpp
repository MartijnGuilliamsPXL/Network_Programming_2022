#include <iostream>
#include <thread>
#include <zmq.hpp>

void benternet( zmq::context_t * context )
{
	zmq::socket_t pusher( *context, ZMQ_PUSH );
	zmq::socket_t subscriber( *context, ZMQ_SUB );

	std::string topic( "wimip>host?>" );
	std::string host( "wimip>host!>tcp://192.168.1.8:24040" );
//	std::string host( "wimip>host!>tcp://benternet.pxl-ea-ict.be:24040" );
//	std::string host( "wimip>host!>tcp://localhost:24040" );

	pusher.connect( "tcp://localhost:24041" );
	subscriber.connect( "tcp://localhost:24042" );
//	pusher.connect( "tcp://benternet.pxl-ea-ict.be:24041" );
//	subscriber.connect( "tcp://benternet.pxl-ea-ict.be:24042" );
	subscriber.setsockopt( ZMQ_SUBSCRIBE, topic.c_str(), topic.length() );

	std::cout << "Connected to Benternet !" << std::endl;

	zmq::message_t * msg = new zmq::message_t();
	while( subscriber.connected() )
	{
		subscriber.recv( msg );
		//std::string message( (char*) msg->data(), msg->size() );
		if( topic.compare( 0, topic.length(), (char*) msg->data() ) )
		{
			pusher.send( host.c_str(), host.length() );
		}
	}

	std::cout << "Disconnected from Benternet !" << std::endl;
}

void wimip( zmq::context_t * context )
{
	zmq::socket_t server( *context, ZMQ_REP );

	server.bind( "tcp://*:24040" );

	std::cout << "Wimip service is started !" << std::endl;

	zmq::message_t * msg = new zmq::message_t();
	while( server.connected() )
	{
		server.recv( msg );
		const char * peerAddress = msg->gets( "Peer-Address" );
		server.send( peerAddress, strlen( peerAddress ) );
	}

	std::cout << "Wimip service has stopped !" << std::endl;
}

int main( int argc, char *argv[] )
{
	try
	{
		zmq::context_t context(1);

		std::thread benternetThread( benternet, &context );
		std::thread wimipThread( wimip, &context );

		benternetThread.join();
		wimipThread.join();
	}
	catch( zmq::error_t & ex )
	{
		std::cerr << "Caught an exception : " << ex.what();
	}

	return 0;
}
