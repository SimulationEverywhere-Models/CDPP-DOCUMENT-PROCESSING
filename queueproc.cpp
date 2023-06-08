/*******************************************************************
*
*  DESCRIPTION: Atomic Model Queueproc
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian modificado por Silvio Musolino 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://smusolin@dc.uba.ar
*
*  DATE: 21/4/2003
*
*******************************************************************/

/** include files **/
#include "queueproc.h"      // class Queue
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )

/** public functions **/

/*******************************************************************
* Function Name: Queue
* Description: 
********************************************************************/
Queueproc::Queueproc( const string &name )
: Atomic( name )
, in( addInputPort( "in" ) )
, stop( addInputPort( "stop" ) )
, done( addInputPort( "done" ) )
, out( addOutputPort( "out" ) )
, empty( addOutputPort( "empty" ) )
, preparationTime( 0, 0, 10, 0 )
{
	string time( MainSimulator::Instance().getParameter( description(), "preparation" ) ) ;

	if( time != "" )
		preparationTime = time ;
}

/*******************************************************************
* Function Name: initFunction
* Description: Resetea la lista
* Precondition: El tiempo del proximo evento interno es Infinito
********************************************************************/
Model &Queueproc::initFunction()
{
	elements.erase( elements.begin(), elements.end() ) ;
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: 
********************************************************************/
Model &Queueproc::externalFunction( const ExternalMessage &msg )
{
	if( msg.port() == in )
	{
		elements.push_back( msg.value() ) ;
		if( elements.size() == 1 )
			holdIn( active, preparationTime );
	}

	if( msg.port() == done )
	{
		elements.pop_front() ;
		if( elements.empty() )
			holdIn( active, Time::Zero );
		else
			holdIn( active, preparationTime );
	}

	if( msg.port() == stop )
		if( state() == active && msg.value() )
		{
			timeLeft = msg.time() - lastChange();
			passivate();
		} else
			if( state() == passive && !msg.value() )
				holdIn( active, timeLeft );

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description: 
********************************************************************/
Model &Queueproc::internalFunction( const InternalMessage & )
{
	passivate();
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: 
********************************************************************/
Model &Queueproc::outputFunction( const InternalMessage &msg )
{
	if ( elements.empty() )
		//Envia el valor 1 representando a TRUE que indica al generador que pare (stop = TRUE)
		sendOutput( msg.time(), empty, 1);
	else
		sendOutput( msg.time(), out, elements.front() );
	
	
	return *this ;
}
