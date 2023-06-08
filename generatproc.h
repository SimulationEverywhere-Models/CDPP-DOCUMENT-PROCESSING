/*******************************************************************
*
*  DESCRIPTION: class Generatorproc
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

#ifndef __GENERATORPROC_H
#define __GENERATORPROC_H

/** include files **/
#include "atomic.h"     // class Atomic
#include "except.h"     // class InvalidMessageException

/** forward declarations **/
class Distribution ;

/** declarations **/
class Generatorproc : public Atomic
{
public:
	Generatorproc( const string &name = "Generatorproc" );				  // Default constructor

	~Generatorproc();

	virtual string className() const
		{return "Generatorproc";}

protected:
	Model &initFunction() ;
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	int 	pid;
	int 	initial, increment;
	Time 	timeLeft;
	
	const Port 	&stop ;
	Port 		&out ;
	
	Distribution 	*dist ;

	Distribution &distribution() {return *dist;}
			
};	// class Generatorproc


#endif   //__GENERATORPROC_H 
