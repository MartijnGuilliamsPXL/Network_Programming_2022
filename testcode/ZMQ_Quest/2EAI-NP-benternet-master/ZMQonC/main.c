#include <stdio.h>
#include <zmq.h>

void * context; //Global context, because you only need one !

int main( int argc, char * argv[] )
{
	printf("Hello World!\n");
	context = zmq_ctx_new();

	void * pusher = zmq_socket( context, ZMQ_PUSH );
	zmq_connect( pusher, "tcp://benternet.pxl-ea-ict.be:24041" );

	zmq_send( pusher, "Hello World", 11, 0 );

	//ALL MAGIC HAPPENS HERE
	printf("Magic!\n");

	zmq_close( pusher );
	zmq_ctx_shutdown( context ); //optional for cleaning lady order (get ready you l*zy f*ck)
	zmq_ctx_term( context ); //cleaning lady goes to work
	printf("KTNXBYE!\n");
	return 0;
}
