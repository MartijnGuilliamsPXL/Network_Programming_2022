#include <iostream>
#include <nzmqt/nzmqt.hpp>
#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>

/*void printMessages( const QList<QByteArray>& messages )
{
	for( const QByteArray & message : messages )
	{
		std::cout << message.toStdString() << std::endl;
	}
}*/

/*nzmqt::ZMQSocket *pusher;
void pingMessage( void )
{
	nzmqt::ZMQMessage message = nzmqt::ZMQMessage( QString("PING").toUtf8() );
	pusher->sendMessage( message );
	std::cout << "PING send !" << std::endl;
}*/

int main( int argc, char *argv[] )
{
	QCoreApplication a(argc, argv);
	std::cout << "Prep!" << std::endl;
	try
	{
		nzmqt::ZMQContext *context = nzmqt::createDefaultContext( &a );
		nzmqt::ZMQSocket *pusher = context->createSocket( nzmqt::ZMQSocket::TYP_PUSH, context );
		nzmqt::ZMQSocket *subscriber = context->createSocket( nzmqt::ZMQSocket::TYP_SUB, context );
		//QObject::connect( subscriber, &nzmqt::ZMQSocket::messageReceived, printMessages );
		QObject::connect( subscriber, &nzmqt::ZMQSocket::messageReceived, []( const QList<QByteArray>& messages )
		{
			std::cout << "Received : " << std::endl;
			for( const QByteArray & message : messages )
			{
				std::cout << message.toStdString() << std::endl;
			}
		} );

		QTimer * pinger = new QTimer( context );
		//QObject::connect( pinger, &QTimer::timeout, pingMessage );
		QObject::connect( pinger, &QTimer::timeout, [pusher](void)
		{
			//QDateTime::currentDateTime().toString( Qt::ISODate );
			nzmqt::ZMQMessage message = nzmqt::ZMQMessage( QDateTime::currentDateTime().toString( Qt::ISODate ).toUtf8() );//QString("PING").toUtf8() );
			pusher->sendMessage( message );
			//std::cout << "PING send !" << std::endl;
		} );
		pinger->setInterval( 1000*10 );

//		pusher->connectTo( "tcp://localhost:24041" );
//		subscriber->connectTo( "tcp://localhost:24042" );
//		pusher->connectTo( "tcp://193.70.81.114:24041" );
//		subscriber->connectTo( "tcp://193.70.81.114:24042" );
		pusher->connectTo( "tcp://benternet.backup.pxl-ea-ict.be:24041" );
		subscriber->connectTo( "tcp://benternet.backup.pxl-ea-ict.be:24042" );
		subscriber->subscribeTo( "" );//QString() );

		if( !pusher->isConnected() || !subscriber->isConnected() )
		{
			std::cerr << "NOT CONNECTED !!!" << std::endl;
		}

		context->start();
		pinger->start();

		/*nzmqt::ZMQMessage * incoming = new nzmqt::ZMQMessage();
		while( subscriber->isConnected() )
		{
			subscriber->receiveMessage( incoming, 0 );
			std::cout << incoming->toByteArray().toStdString() << std::endl;
		}*/
	}
	catch( nzmqt::ZMQException & ex )
	{
		std::cerr << "Caught an exception : " << ex.what();
	}
	std::cout << "Start!" << std::endl;
	return a.exec();
}
