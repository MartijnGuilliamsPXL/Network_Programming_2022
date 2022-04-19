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
            std::cout << std::string( (char*) msg->data()) << std::endl;
            if((msg->size()) == 3){
                std::cout << "size is 3" << std::endl;
            }
			std::cout << "Subscribed : [" << std::string( (char*) msg->data(), msg->size() ) << "]" << std::endl;

            //subscriber.send( "test>", 5 );
        }
	}
	catch( zmq::error_t & ex )
	{
		std::cerr << "Caught an exception : " << ex.what();
    }

	return 0;
}
