/*******************************************************************
*
*  DESCRIPTION: Atomic Model Queueproc
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian modificada por Silvio Musolino 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://smusolin@dc.uba.ar
*
*  DATE: 21/4/2003
*
*******************************************************************/

#ifndef __QUEUEPROC_H
#define __QUEUEPROC_H

#include <list>
#include "atomic.h"     // class Atomic

class Queueproc : public Atomic
{
public:
	Queueproc( const string &name = "Queueproc" );					//Default constructor

	virtual string className() const
		{return "Queueproc";}
			
protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port 		&in, &stop, &done;
	Port 			&out, &empty;
	
	Time preparationTime;
	typedef list<Value> ElementList ;
	ElementList elements ;

	Time timeLeft;

};	// class Queueproc


#endif   //__QUEUEPROC_H
