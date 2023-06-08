/*******************************************************************
*
*  DESCRIPTION: Atomic Model Scan
*
*  AUTHOR: Silvio Jose Musolino
*
*  EMAIL: mailto://smusolin@dc.uba.ar
*
*
*  DATE: 26/4/2003
*
*******************************************************************/

/** include files **/
#include <math.h>        // fabs( ... )
#include "scan.h"           // base header
#include "message.h"       // InternalMessage ....
#include "distri.h"        // class Distribution
#include "mainsimu.h"      // class MainSimulator
#include "strutil.h"       // str2float( ... )

/*******************************************************************
* Function Name: Scan
* Description: constructor
********************************************************************/
Scan::Scan( const string &name )
	: Atomic( name )
	, lote( addInputPort( "lote" ) )
	, fin( addInputPort( "fin" ) )
	, colavacia( addInputPort( "colavacia" ) )
	, doc( addOutputPort( "doc" ) )
	, tidle( addOutputPort( "tidle" ) )
	, tomarlote( addOutputPort( "tomarlote" ) )
	, cantdoc( addOutputPort( "cantdoc" ) )
	, finished( addOutputPort( "finished" ) )
{
	//carga de parametros
	//Tiempo de carga de cabecera
	string tiempoCC(MainSimulator::Instance().getParameter( description(), "tiempo_carga_cabecera" ) );
	if( tiempoCC != "" )
		tiempo_carga_cabecera = tiempoCC;
	else
		tiempo_carga_cabecera = Time(0,0,5,0);

	//Tiempo de procesamiento del documento
	string tiempoPD( MainSimulator::Instance().getParameter( description(), "tiempo_proc_doc" ) );
	if( tiempoPD != "" )
		tiempo_proc_doc = tiempoPD;
	else
		tiempo_proc_doc = Time(0,0,0,1);
	
}


/*******************************************************************
* Function Name: initFunction
* Description : Setea a Scan en estado libre y espera la llegada 
		del primer lote de docmentos.
********************************************************************/
Model &Scan::initFunction()
{
	//Contadores
	docs_a_scan = 0;
	tiempo_ocioso = 0;
	nro_lote = 0;
	cant_doc = 0;
	
	hora_que_para = Time(0,0,0,0);
	
	//Flags
	fin_proc = FALSE;
	tomar_lote = FALSE;
	enviar_doc = FALSE;
	cola_vacia = FALSE;
	
	this->passivate();
	return *this ;
}


/*******************************************************************
* Function Name: externalFunction 
********************************************************************/
Model &Scan::externalFunction( const ExternalMessage &msg )
{
	//Cuando llega un lote
	if( msg.port() == lote)
	{  
		//Si no hay documento a escanear
   	if (docs_a_scan == 0)
      {  
      	if ( cola_vacia )
			{	
				//incrementa el contador de tiempo ocioso.
      		tiempo_ocioso += (int)((msg.time().asMsecs() - hora_que_para.asMsecs())/1000);
      
				//Actualiza el falg de cola vacia
				cola_vacia = FALSE;
			}
			
			//incremento el nro de lote
			nro_lote++;
		
        	//Setea la cantidad de documentos a escanear
        	docs_a_scan = (int)(msg.value());
        	  	
        	//Sch del tiempo de cabecera
      	holdIn(active, tiempo_carga_cabecera);
      }
   }
    
   //Si llega el fin de procesamiento
   if ( msg.port() == fin) 
   {
   	fin_proc = TRUE;	//enciende el flag de fin de procesamiento
   	
   	//Si no hay lotes encolados
		if ( cola_vacia )
		{
			//incrementa el contador de tiempo ocioso. 
      	tiempo_ocioso += (int)((msg.time().asMsecs() - hora_que_para.asMsecs()) / 1000);
      	
      	//Para que emita la salida
			holdIn(active, Time::Zero);
		}	
   }
      
   //Si llega colavacia
   if ( msg.port() == colavacia )
   {
 		//anota la hora en que se detuvo
	   hora_que_para = msg.time();
	   
	   //Pone el falg de cola vacia en true
		cola_vacia = TRUE;
		
		//Para que emita la salida   
		holdIn(active, Time::Zero);  
   }
           
	return *this;
}

/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &Scan::internalFunction( const InternalMessage &msg )
{
	//Si esta prendido el  flag de fin de procesamiento lo apago
	if ( fin_proc )
		fin_proc = FALSE;
		
	
	//Si tomo un lote
	if ( tomar_lote )
	{
		//Apaga el flag
		tomar_lote = FALSE;
		
		//aguarda a que la cola se lo envie
		passivate();		
	}
	else
   {   	
	   //Si envio documento 
	   if ( enviar_doc )
	   {
	   	//Incfrementa el contador de documentos
	   	cant_doc++;
	   	
	   	//Descrementa la cantidad de documentos a escanear
   		docs_a_scan--;
   		
   		//Apaga el flag
   		enviar_doc = FALSE;
	   }
	   
	   //si quedan documentos para procesar
   	if (docs_a_scan > 0 )
   	{    	
   		//Indicar que se debe enviar un documento
   		enviar_doc = TRUE;
   			    			
   		//Comienza a escanear documentos
   		holdIn(active, tiempo_proc_doc);
   	}
   	else
   	{
   		if ( !cola_vacia )
   		{
   			//Pide el proximo lote mediante la funcion de salida
	   		tomar_lote = TRUE;
	   	
	   		//aguarda a que la cola se lo envie
				holdIn(active, Time::Zero);
			}
			else
				//Espera a recibir la señal de fin o que la simulacion finalize por falta de eventos
				passivate();
	   }
	}
	

	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
********************************************************************/

Model &Scan::outputFunction( const InternalMessage &msg )
{	
	//si tiene que tomar el lote
	if ( tomar_lote == TRUE )
	{
		sendOutput( msg.time(), tomarlote, nro_lote + 1);
	}
	
	//si tiene 	que enviar documento
	if ( enviar_doc == TRUE )
   {    
    	sendOutput( msg.time(), doc, nro_lote);
   }
   
   
   //Si no hay mas lotes para procesar
   if ( cola_vacia )
   {
   	//Emite los resultados
    	sendOutput( msg.time(), tidle, tiempo_ocioso);
    	
    	//Emite la cantidad de documentos
   	sendOutput( msg.time(), cantdoc, cant_doc);
    }
   
   //Si llego la señal de fin de procesamiento
   if ( fin_proc )
   	//Emite la señal de fin de procesamiento	
    	sendOutput( msg.time(), finished, 1);
   	
   
	return *this ;
}
	
	