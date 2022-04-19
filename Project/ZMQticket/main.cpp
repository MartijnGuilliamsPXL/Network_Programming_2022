#include <iostream>
#include <nzmqt/nzmqt.hpp>
#include <QCoreApplication>
#include <QtConcurrent>
#include "benternet.h"
#include <windows.h> // for Sleep

#define SELFHEALING

extern void runTicket( nzmqt::ZMQSocket * server )
{
	static quint64 ticketNumber = 0;
	try
	{
		std::cout << "Starting ticket service..." << std::endl;
		server->bindTo( "tcp://*:24043" );
		while( server->isConnected() )
		{
			server->receiveMessage( (nzmqt::ZMQSocket::ReceiveFlags) 0 );
			std::cout << "Ticket request received !" << std::endl;
			server->sendMessage( QByteArray::number( ticketNumber ) );
			ticketNumber++;
		}
		std::cout << "Stopping ticket service..." << std::endl;
		server->close();
	}
	catch( nzmqt::ZMQException & ex )
	{
		std::cerr << "Catched an exception : " << ex.what();
	}
}

void setupTicketService( nzmqt::ZMQContext *context )
{
	try
	{
		nzmqt::ZMQSocket * server = context->createSocket( nzmqt::ZMQSocket::TYP_REP, context );
#ifdef SELFHEALING
		QFutureWatcher<void> * watcher = new QFutureWatcher<void>( context );
		QObject::connect( watcher, &QFutureWatcher<void>::finished, [watcher, server]()
		{
			QFuture<void> future = QtConcurrent::run( runTicket, server );
			watcher->setFuture( future );
		} );
		QFuture<void> future = QtConcurrent::run( runTicket, server );
		watcher->setFuture( future );
#else
		QtConcurrent::run( runTicket, server ); //Not robust aka not self healing
#endif //SELFHEALING
	}
	catch( nzmqt::ZMQException & ex )
	{
		std::cerr << "Catched an exception : " << ex.what();
	}
}

QByteArray getRemoteAddress( Benternet * b )
{
	b->subscribe( "wimip>host!>" );
	Sleep( 1000 ); //To freaking fast...
	std::cout << "Requesting wimip address via benternet" << std::endl;
	b->send( "wimip>host?>" );
	std::cout << "Waiting for response..." << std::endl << std::flush;
	QByteArray response = b->receive();
	std::cout << "Received : " << response.toStdString() << std::endl << std::flush;
	QString host = response.split( '>' ).at( 2 ); //not safe, but we rather crash !
	std::cout << "Extracted : " << host.toStdString() << std::endl;
	b->unsubscribe( "wimip>host!>" );
	nzmqt::ZMQSocket * client = b->getContext()->createSocket( nzmqt::ZMQSocket::TYP_REQ, b->getContext() );
	std::cout << "Setting up a direct request connection to " << host.toStdString() << std::endl;
	client->connectTo( host );
	std::cout << "Sending an empty request..." << std::endl;
	client->sendMessage( QByteArray( "?" ), 0 ); //Do wait for successful send
	std::cout << "Waiting for response..." << std::endl;
	QByteArray myExternalIP = client->receiveMessage( (nzmqt::ZMQSocket::ReceiveFlags) 0 ).first(); //Do wait for receive
	std::cout << "Received : " << myExternalIP.toStdString() << std::endl;
	client->close();
	client->deleteLater();
	return( myExternalIP );
}

int main( int argc, char *argv[] )
{
	QCoreApplication a( argc, argv );
	std::cout << "Prep!" << std::endl;
	try
	{
		Benternet * b = new Benternet( &a );
		b->start();
		QByteArray host( "ticket>host!>tcp://" );
		host.append( getRemoteAddress( b ) ).append( ":24043" );
		setupTicketService( b->getContext() );
		QObject::connect( b, &Benternet::receiving, [b, host]( const QByteArray & message )
		{
			QByteArrayList parts = message.split( '>' );
			if( parts.at( 1 ) == "host?" )
			{
				std::cout << "Host request received !" << std::endl;
				b->send( host );
			}
		} );
		b->subscribe( "ticket>host?>" );
	}
	catch( nzmqt::ZMQException & ex )
	{
		std::cerr << "Caught an exception : " << ex.what();
	}
	std::cout << "Start!" << std::endl;
	return a.exec();
}
