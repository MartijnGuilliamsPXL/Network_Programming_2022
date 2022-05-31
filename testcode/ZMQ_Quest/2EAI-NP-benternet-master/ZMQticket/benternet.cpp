#include "benternet.h"


void Benternet::prepare()
{
	pusher = context->createSocket( nzmqt::ZMQSocket::TYP_PUSH, context );
	subscriber = context->createSocket( nzmqt::ZMQSocket::TYP_SUB, context );
//	pusher->connectTo( "tcp://localhost:24041" );
//	subscriber->connectTo( "tcp://localhost:24042" );
//	pusher->connectTo( "tcp://193.70.81.114:24041" );
//	subscriber->connectTo( "tcp://193.70.81.114:24042" );
	pusher->connectTo( "tcp://benternet.pxl-ea-ict.be:24041" );
	subscriber->connectTo( "tcp://benternet.pxl-ea-ict.be:24042" );
	QObject::connect( subscriber, &nzmqt::ZMQSocket::messageReceived, this, &Benternet::messageReceived );
}
