#ifndef BENTERNET_H
#define BENTERNET_H

#include <iostream>
#include <QObject>
#include <nzmqt/nzmqt.hpp>

class Benternet : public QObject
{
	Q_OBJECT

public:
	Benternet( QObject * parent = nullptr )		: QObject(parent)						{ context = nzmqt::createDefaultContext( this ); prepare(); }
	Benternet( nzmqt::ZMQContext * context )	: QObject(context), context(context)	{ prepare(); }

	nzmqt::ZMQContext * getContext( void) const	{ return( context ); }
	QByteArray receive( void )					{ return( subscriber->receiveMessage( (nzmqt::ZMQSocket::ReceiveFlags) 0 ).first() ); }

signals:
	void receiving( const QByteArray & message );

public slots:
	void start( void )											{ context->start(); if( !pusher->isConnected() || !subscriber->isConnected() ) { std::cerr << "NOT CONNECTED !!!" << std::endl; } }
	bool send( const char * message )							{ return( pusher->sendMessage( QString( message ).toUtf8() ) ); }
	bool send( const QString & message )						{ return( pusher->sendMessage( message.toUtf8() ) ); }
	bool send( const QByteArray & message )						{ return( pusher->sendMessage( message ) ); }
	bool send( nzmqt::ZMQMessage & message )					{ return( pusher->sendMessage( message ) ); }
	void subscribe( const char * topic )						{ subscriber->subscribeTo( topic ); }
	void subscribe( const QString & topic )						{ subscriber->subscribeTo( topic ); }
	void subscribe( const QByteArray & topic )					{ subscriber->subscribeTo( topic ); }
	void unsubscribe( const char * topic )						{ subscriber->unsubscribeFrom( topic ); }
	void unsubscribe( const QString & topic )					{ subscriber->unsubscribeFrom( topic ); }
	void unsubscribe( const QByteArray & topic )				{ subscriber->unsubscribeFrom( topic ); }

private slots:
	void messageReceived( const QList<QByteArray> & messages )	{ for( const QByteArray & message : messages ) { emit( receiving( message ) ); } }

private:
	void prepare( void );

	nzmqt::ZMQContext * context;
	nzmqt::ZMQSocket * pusher;
	nzmqt::ZMQSocket * subscriber;
};

#endif // BENTERNET_H
