/*******************************************************************
*
*  DESCRIPTION: class Generatorproc
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian modificado por Silvio
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://smusolin@dc.uba.ar
*
*  DATE: 21/4/2003
*
*******************************************************************/

/** include files **/
#include "generatproc.h"       // base header
#include "message.h"       // class InternalMessage 
#include "mainsimu.h"      // class Simulator
#include "distri.h"        // class Distribution 
#include "strutil.h"       // str2Value( ... )

/*******************************************************************
* Function Name: Generatorproc
* Description: constructor
********************************************************************/
Generatorproc::Generatorproc( const string &name )
: Atomic( name )
, stop( addInputPort( "stop" ) )
, out( addOutputPort( "out" ) )
{
	try
	{
		dist = Distribution::create( MainSimulator::Instance().getParameter( description(), "distribution" ) );
		MASSERT( dist ) ;
		for ( register int i = 0; i < dist->varCount(); i++ )
		{
			string parameter( MainSimulator::Instance().getParameter( description(), dist->getVar( i ) ) ) ;
			dist->setVar( i, str2Value( parameter ) ) ;
		}

		if( MainSimulator::Instance().existsParameter( description(), "initial" ) )
			initial = str2Int( MainSimulator::Instance().getParameter( description(), "initial" ) );
		else
			initial = 0;

		if( MainSimulator::Instance().existsParameter( description(), "increment" ) )
			increment = str2Int( MainSimulator::Instance().getParameter( description(), "increment" ) );
		else
			increment = 1;

	} catch( InvalidDistribution &e )
	{
		e.addText( "The model " + description() + " has distribution problems!" ) ;
		e.print(cerr);
		MTHROW( e ) ;
	} catch( MException &e )
	{
		MTHROW( e ) ;
	}
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &Generatorproc::initFunction()
{
	pid = initial;
	holdIn( active, Time::Zero ) ;
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
********************************************************************/
Model &Generatorproc::externalFunction( const ExternalMessage &msg )
{
	
	if( msg.port() == stop )
		if( state() == active && msg.value() )
		{
			timeLeft = msg.time() - lastChange();
			passivate();
		} else
			if( state() == passive && !msg.value() )
				holdIn( active, timeLeft );
	
	return *this ;
}


/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &Generatorproc::internalFunction( const InternalMessage & )
{
	holdIn( active, Time( static_cast< float >( fabs( distribution().get() ) ) ) ) ;
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
********************************************************************/
Model &Generatorproc::outputFunction( const InternalMessage &msg )
{
	sendOutput( msg.time(), out, pid ) ;
	pid += increment;
	return *this ;
}

Generatorproc::~Generatorproc()
{
	delete dist;
}
