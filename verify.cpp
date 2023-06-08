/*******************************************************************
*
*  DESCRIPTION: Atomic Model Verify
*
*  AUTHOR: Silvio Jose Musolino
*
*  EMAIL: mailto://smusolin@dc.uba.ar
*
*
*  DATE: 21/4/2003
*
*******************************************************************/

/** include files **/
#include <math.h>        // fabs( ... )
#include "verify.h"     // base header
#include "message.h"       // InternalMessage ....
#include "distri.h"        // class Distribution
#include "mainsimu.h"      // class MainSimulator
#include "strutil.h"       // str2float( ... )

//Declaraciones del Tp1
#include "tp1dec.h"

/*******************************************************************
* Function Name: Verify
* Description: constructor
********************************************************************/
Verify::Verify( const string &name )
	: Atomic( name )
	, doc( addInputPort( "doc" ) )
	, rechazo( addInputPort( "rechazo" ) )
	, fin( addInputPort( "fin" ) )
	, colavacia( addInputPort( "colavacia" ) )
	, doct( addOutputPort( "doct" ) )
	, tidle( addOutputPort( "tidle" ) )
	, cantdocb( addOutputPort( "cantdocb" ) )
	, cantdoct( addOutputPort( "cantdoct" ) )
	, arrancar( addOutputPort( "arrancar" ) )
{
	//carga de parametros
	//Tiempo de carga de cabecera
	string tiempoV(MainSimulator::Instance().getParameter( description(), "tiempo_verify" ) );
	if( tiempoV != "" )
		tiempo_verify = tiempoV;
	else
		tiempo_verify = Time(0,0,10,0);
		
	//Porcentaje de rechazp
	string porcR(MainSimulator::Instance().getParameter( description(), "porc_rechazo" ) );
	if( porcR != "" )
		porc_rechazo = str2Int(porcR);
	else
		porc_rechazo = 2;
		
	//Tiempo de busqueda
	string tiempoB(MainSimulator::Instance().getParameter( description(), "tiempo_busqueda" ) );
	if( tiempoB != "" )
		tiempo_busqueda = tiempoB;
	else
		tiempo_busqueda = Time(0,2,0,0);
	
}


/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &Verify::initFunction()
{
	//Contadores
	tiempo_ocioso = 0;
	nro_lote = 0;
	cant_doc = 0;
	cant_rechazo = 0;
	
	//flags de tiempo
	hora_que_para = Time(0,0,0,0);
	
	//Flags
	enviar_doc = FALSE;
	cola_vacia = TRUE;
	doc_rechazado = FALSE;
	arrancar_generador = FALSE;
		
	this->passivate();
	return *this ;
}


/*******************************************************************
* Function Name: externalFunction 
********************************************************************/
Model &Verify::externalFunction( const ExternalMessage &msg )
{
	//Cuando llega un documento
	if( msg.port() == doc)
	{  
		//Enciende el flag para enviar
		enviar_doc = TRUE;
		
		//incrementa la cantidad de documentos
		cant_doc++;
		
		
		if ( cola_vacia )
		{
			//incrementa el contador de tiempo ocioso. Tiene que descontar el tiempo de procesamiento del ultimo documento
      	tiempo_ocioso += (int)((msg.time().asMsecs() - hora_que_para.asMsecs() - tiempo_verify.asMsecs())/1000);
      
			//Actualiza el falg de cola vacia
			cola_vacia = FALSE;
			
			//Flag para enviar señal de arrenque
			arrancar_generador = TRUE;
		}
		
      //Toma el nro de lote del documento
      nro_lote = (int)(msg.value());
		
		//Si es rechazado	
		if ( doc_rechazado )
			//Comienza a buscar el documento entre la pila de papeles
			holdIn(active, tiempo_busqueda);	
		else
			//Para que emita la salida
			holdIn(active, Time::Zero);
		
	}
		
	//Si llega una señal de rechazo y no esta encendido rechazo
	if ( msg.port() == rechazo && !doc_rechazado)	
	{
		//Enciende el flag de rechazo para el documento que se encuentra actualmente procesando si no supera el 10 % de los documentos procesados
		doc_rechazado = (cant_rechazo <= (porc_rechazo * cant_doc / 100));
	}
	
	//Si se advierte que la cola esta vacia
	if ( msg.port() == colavacia )
	{
		//anota la hora en que se detuvo
	   hora_que_para = msg.time();
	   
	   //Pone el falg de cola vacia en true
		cola_vacia = TRUE;
		
		//Para que emita la salida   
		holdIn(active, Time::Zero);  

	}  
	
	//Si llega el mensaje de fin
	if ( msg.port() == fin )
	{	
		//Si no hay documentos encolados
		if ( cola_vacia )
		{
			//incrementa el contador de tiempo ocioso. Tambien tiene que descontar el tiempo de procesamiento del ultimo documento
      	tiempo_ocioso += (int)((msg.time().asMsecs() - hora_que_para.asMsecs())/1000);
      	
      	//Para que emita la salida
			holdIn(active, Time::Zero);
		}	
	}
	
	
	return *this;
}

/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &Verify::internalFunction( const InternalMessage &msg )
{
	//Si se encuentra procesando un documento
	if ( enviar_doc )
	{	
		//Apaga el flag
		enviar_doc = FALSE;
		
		//Verifica si va a rechazo o no
		if ( doc_rechazado )
		{	
			//Apaga el falg de rechazo
			doc_rechazado = FALSE;
			
			//Incrementa el contador de rechazos
			cant_rechazo++;
		}	
	}
	
	//Si estaba encendido el falg de arrancar lo apago
	if ( arrancar_generador )
		arrancar_generador = FALSE;
	
	//Espera el proximo evento externo
	passivate();	
	
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
********************************************************************/

Model &Verify::outputFunction( const InternalMessage &msg )
{	
	
	//Si tiene que enviar un docuemnto
	if ( enviar_doc )
		sendOutput( msg.time(), doct, nro_lote);
	
	//si cola vacia entonces envia los mensajes de status
	if ( cola_vacia )
	{
		//informa el tiempo ocioso
		sendOutput( msg.time(), tidle, tiempo_ocioso);
		
		//informa la cantidad de documentos verificados
		sendOutput( msg.time(), cantdoct, (cant_doc - cant_rechazo) );
		
		//informa la cantidad de documentos enviados a verify
		sendOutput( msg.time(), cantdocb, cant_rechazo);
	}
	
	//Si esta encendido el flag envia la señal de arranque
	if ( arrancar_generador )
		//Envia el valor 0 representando el false para indicarle a la cola o el generador que no hagan stop
		sendOutput( msg.time(), arrancar, 0);
	
	return *this ;
}
	
	