/*******************************************************************
*
*  DESCRIPTION: class Scan 
*
*  AUTHOR: Silvio Jose Musolino
*
*  EMAIL: mailto://smusolin@dc.uba.ar
*
*  DATE: 26/04/2003
*
*******************************************************************/

#ifndef __SCAN_H
#define __SCAN_H

/** include files **/
#include "atomic.h"     	// class Atomic
#include "tp1dec.h"
/** forward declarations **/

class Scan: public Atomic
{
public:

	Scan( const string &name = "Scan" );				  // Default constructor

	virtual string className() const
		{return "Scan";}

protected:
	Model &initFunction() ;
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
   // Parametros
   Time		tiempo_carga_cabecera, tiempo_proc_doc;
	
   //Variables internas
	int   	docs_a_scan, tiempo_ocioso, nro_lote, cant_doc; 	//contadores
	int		fin_proc, tomar_lote, enviar_doc, cola_vacia; 	//flags
	Time	hora_que_para;	//indica la hora en que para el scan. Se usa para calcular tidle
	
	//Puertos de Entrada
	const Port  	&lote, &fin, &colavacia;

  	//Puerto de Salida
	Port			&doc, &tidle, &tomarlote, &cantdoc, &finished;

};	// class Scan



#endif   //__SCAN_H 
