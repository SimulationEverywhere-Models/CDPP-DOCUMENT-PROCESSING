/*******************************************************************
*
*  DESCRIPTION: class Verify 
*
*  AUTHOR: Silvio Jose Musolino
*
*  EMAIL: mailto://smusolin@dc.uba.ar
*
*  DATE: 21/04/2003
*
*******************************************************************/

#ifndef __VERIFY_H
#define __VERIFY_H

/** include files **/
#include "atomic.h"     	// class Atomic
#include "tp1dec.h"


/** forward declarations **/

class Verify: public Atomic
{
public:

	Verify( const string &name = "Verify" );				  // Default constructor

	virtual string className() const
		{return "Verify";}

protected:
	Model &initFunction() ;
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
   // Parametros
   Time		tiempo_verify, tiempo_busqueda;
   int 		porc_rechazo;
	
   //Variables internas
	int   	tiempo_ocioso, nro_lote, cant_doc, cant_rechazo; 	//contadores
	int		enviar_doc, cola_vacia, doc_rechazado, arrancar_generador; 	//flags
	Time	hora_que_para;	//indica la hora en que para el scan. Se usa para calcular tidle
	
	//Puertos de Entrada
	const Port 		&doc, &rechazo, &fin, &colavacia;

  	//Puerto de Salida
	Port			&doct, &tidle, &cantdocb, &cantdoct, &arrancar;

};	// class Interpret



#endif   //__VERIFY_H
