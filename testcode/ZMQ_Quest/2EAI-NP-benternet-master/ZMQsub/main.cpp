#include <iostream>
#include <string>
#include <zmq.hpp>

int main( void )
{

    try
	{
		zmq::context_t context(1);

		//Incoming messages come in here
        zmq::socket_t subscriber( context, ZMQ_SUB );
        //subscriber.connect( "tcp://localhost:24042" );
        subscriber.connect( "tcp://benternet.pxl-ea-ict.be:24042" );
        subscriber.setsockopt( ZMQ_SUBSCRIBE, "Zeeslag>", 8 );

		zmq::message_t * msg = new zmq::message_t();
		while( subscriber.connected() )
		{
			subscriber.recv( msg );
			std::cout << "Subscribed : [" << std::string( (char*) msg->data(), msg->size() ) << "]" << std::endl;
        }
	}
	catch( zmq::error_t & ex )
	{
		std::cerr << "Caught an exception : " << ex.what();
    }

	return 0;
}
