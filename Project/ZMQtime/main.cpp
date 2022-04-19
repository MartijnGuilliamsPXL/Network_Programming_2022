#include <iostream>
#include <nzmqt/nzmqt.hpp>
#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>

int main( int argc, char *argv[] )
{
	QCoreApplication a(argc, argv);
	try
	{
		nzmqt::ZMQContext *context = nzmqt::createDefaultContext( &a );
		nzmqt::ZMQSocket *pusher = context->createSocket( nzmqt::ZMQSocket::TYP_PUSH, context );
		nzmqt::ZMQSocket *subscriber = context->createSocket( nzmqt::ZMQSocket::TYP_SUB, context );

		QObject::connect( subscriber, &nzmqt::ZMQSocket::messageReceived, [pusher]( const QList<QByteArray>& messages )
		{
			for( const QByteArray & message : messages )
			{
				QByteArrayList parts = message.split( '>' );
				if( parts.at( 1 ) == "get" )
				{
					nzmqt::ZMQMessage message = nzmqt::ZMQMessage( QString( "time>is>" ).append( QDateTime::currentDateTime().toString( Qt::ISODate ) ).toUtf8() );
					pusher->sendMessage( message );
				}
				else if( parts.at( 1 ) == "send" )
				{
					int duration = 0;
					int location = 4;
					if( parts.at( 2 ) == "on" )
					{
						duration = QDateTime::currentDateTime().msecsTo( QDateTime::fromString( QString::fromUtf8( parts.at( 3 ) ) ) ) / 1000;
					}
					else if( parts.at( 2 ) == "onFormatted" )
					{
						duration = QDateTime::currentDateTime().msecsTo( QDateTime::fromString( QString::fromUtf8( parts.at( 4 ) ), QString::fromUtf8( parts.at( 3 ) ) ) );
						location = 5;
					}
					else if( parts.at( 2 ) == "in" )
					{
						duration = parts.at( 3 ).toInt();
					}
					if( duration > 0 )
					{
						QByteArray message = parts.mid( location ).join( '>' );
						QTimer::singleShot( duration * 1000, [pusher, message]() { pusher->sendMessage( message ); } );
					}
				}
			}
		} );

//		pusher->connectTo( "tcp://localhost:24041" );
//		subscriber->connectTo( "tcp://localhost:24042" );
//		pusher->connectTo( "tcp://193.70.81.114:24041" );
//		subscriber->connectTo( "tcp://193.70.81.114:24042" );
		pusher->connectTo( "tcp://benternet.pxl-ea-ict.be:24041" );
		subscriber->connectTo( "tcp://benternet.pxl-ea-ict.be:24042" );
		subscriber->subscribeTo( "time>get>" );
		subscriber->subscribeTo( "time>send>" );

		context->start();

		if( !pusher->isConnected() || !subscriber->isConnected() )
		{
			std::cerr << "NOT CONNECTED !!!" << std::endl;
		}
	}
	catch( nzmqt::ZMQException & ex )
	{
		std::cerr << "Caught an exception : " << ex.what();
	}
	return a.exec();
}
