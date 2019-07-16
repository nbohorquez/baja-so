/*
******************************************************************************
* Universidad Simon Bolivar                                                  *
* BAJA SAE USB                                                               *
* Direccion Tecnica					                                                 *
* Division de Electronica									                                   *
*                                                                            *
*                                                                            *
*                 SISTEMA OPERATIVO DEL PROTOTIPO 2007-2008                  *
*									                                                           *
* ARCHIVO: demonios.c                                                        *
*                                                                            *
*                                                                            *
* Alejandro Maita                                                            *
* Andres Arguello                                                            *
* Bruno Pierucci                                                             *
* Daniel Bello							                                                 *
* Nestor Bohorquez        						                                       *
*                                              Diciembre 2007 - Febrero 2008 *
******************************************************************************
*/

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "procesos.h"                                        

/*
******************************************************************************
*                     VARIABLES GLOBALES DE LOS DEMONIOS                     *
******************************************************************************
*/

/* Definiciones para DEMONIO ControlarTablero */
#define TABLERO_TIEMPO_ACTUALIZACION    120
#define TABLERO_MOSTRAR_VELOCIDAD       0
#define TABLERO_MOSTRAR_RPM             1
#define TABLERO_MOSTRAR_GASOLINA        2
#define TABLERO_MOSTRAR_BATERIA         3
#define TABLERO_APAGAR                  (PTC_PTC1 = 1)
#define TABLERO_PRENDER                 (PTC_PTC1 = 0)
#define TABLERO_PRENDIDO                (!PTC_PTC1)

unsigned char ControlarTableroEstado = TABLERO_MOSTRAR_VELOCIDAD;
unsigned char TableroActualizar = 0;
unsigned char TableroVelocidad = 0;
unsigned char TableroGasolina = 0;
unsigned char TableroRPM = 0;
unsigned char TableroBateria = 0;
unsigned char TableroLED = 0;

/* Definiciones para DEMONIO OsciladorFrenos */
#define LUZ_EMERGENCIA          1200

char OsciladorEstadoPTC3 = 0;
char OsciladorEstadoPTC2 = 0;
int OsciladorTemp = 0;

/* Este es el cosito que se quiere */
extern CARRO BajaSAEUSB;

/*
******************************************************************************
* 				                   CODIGO DE LOS DEMONIOS                          *
******************************************************************************
*/

void ControlarTablero(void) @ "CODIGO__DEMONIOS" {
  /*
    ControlarTablero hace la funcion de lo que antes eran tres tareas 
    distintas:
    
    -> ActualizarTablero: Cada cierto tiempo (250ms aproximadamente) esta
       funcion lee las variables de BajaSAEUSB (velocidad, RPM, bateria, 
       y gasolina) y las opera de manera adecuada para poder visualizarse 
       en el tablero.
    -> ModoDeFuncionamiento: Hay un interruptor en el tablero que indica 
       si el sistema va a funcionar de forma automatica o manual. En modo 
       automatico el tablero se apaga si las RPM disminuyen de cierto 
       valor. Si esta en modo manual, el tablero nunca se apaga.
    -> ControlarTablero: Toma las variables de ActualizarTablero y las muestra
       en el tablero.
  */  
    
  /* 
    Mira primero si las revoluciones del motor son suficientes y que el modo
    automatico esta encendido 
  */
  if((BajaSAEUSB.RPM < 1000) && (CARRO_MODO_AUTO)) {
    if(TABLERO_PRENDIDO)
      TABLERO_APAGAR;
    return;
  } 
  else {
    if(!TABLERO_PRENDIDO)
      TABLERO_PRENDER;
    
    if(TableroActualizar == TABLERO_TIEMPO_ACTUALIZACION) {
      TableroActualizar = 0;
     
      TableroVelocidad = BajaSAEUSB.Velocidad/5;
      if(TableroVelocidad > 12)
        TableroVelocidad = 12;
      
      TableroGasolina = (char)(BajaSAEUSB.Estado&CARRO_ESTADO_MASCARA_GAS)>>2;
      TableroBateria = (char)(BajaSAEUSB.Estado&CARRO_ESTADO_MASCARA_BAT)>>5;
      
      if(BajaSAEUSB.RPM < 1000)
        TableroRPM = 0;
      else if(BajaSAEUSB.RPM > 3400)
        TableroRPM = 14;
      else 
        TableroRPM = (char)((BajaSAEUSB.RPM-800)/200)+1;
    }
    else
      TableroActualizar++;  
     
    switch(ControlarTableroEstado) { 
      case TABLERO_MOSTRAR_VELOCIDAD:
        if(TableroLED<TableroVelocidad) {
          PTB = TableroLED;
        	PTC_PTC4 = 1;
      	  PTC_PTC4 = 0;
      	  TableroLED++;                  
        } 
        else {
          TableroLED = 1;
          ControlarTableroEstado = TABLERO_MOSTRAR_RPM;
        }
        break;
      case TABLERO_MOSTRAR_RPM:
        if(TableroRPM == 0) {
          PTA = 0;
          PTC_PTC4 = 1;
     		  PTC_PTC4 = 0;
     		  TableroLED = 0;
    		  ControlarTableroEstado = TABLERO_MOSTRAR_GASOLINA;
     	  } 
     	  else {
     	    if(TableroLED<TableroRPM) {
            PTA = TableroLED;
        	  PTC_PTC4 = 1;
      	    PTC_PTC4 = 0;
      	    TableroLED++;
          }
          else {
            TableroLED = 0;
            ControlarTableroEstado = TABLERO_MOSTRAR_GASOLINA;
          }
        }
        break;
      case TABLERO_MOSTRAR_GASOLINA:
        if(TableroGasolina == 0) {
       	  PTB = 12;
     		  PTC_PTC4 = 1;
     		  PTC_PTC4 = 0;
    		  ControlarTableroEstado = TABLERO_MOSTRAR_BATERIA;
     	  } 
     	  else {
       		if(TableroLED<TableroGasolina) {
        		if(TableroLED<3)
              PTB = TableroLED+13;
      		  else  // TableroLED == 3
           		PTA = 14;
            
      		  PTC_PTC4 = 1;
      			PTC_PTC4 = 0;
      			TableroLED++;
          } 
          else {
            TableroLED = 0;
            ControlarTableroEstado = TABLERO_MOSTRAR_BATERIA;
          }
        }
        break;
      case TABLERO_MOSTRAR_BATERIA:
        if(TableroBateria <= 2) {
    		  PTA = 15;
    		  PTC_PTC4 = 1;
    		  PTC_PTC4 = 0;    
    		}
    		ControlarTableroEstado = TABLERO_MOSTRAR_VELOCIDAD;
    		break;
    	default:
    	  break;
    }
  }
}

void OsciladorFrenos(void) @ "CODIGO__DEMONIOS" {
  OsciladorEstadoPTC2 = !OsciladorEstadoPTC2;
  PTC_PTC2 = OsciladorEstadoPTC2;
 
  // Comprueba si es tiempo de cambiar la luz de emergencia
  if(OsciladorTemp == LUZ_EMERGENCIA) {
    OsciladorEstadoPTC3 = !OsciladorEstadoPTC3;     
    OsciladorTemp = 0;     // reinicia contador 
  }
  else 
    OsciladorTemp++;  // incrementa contador

  if(!OsciladorEstadoPTC3)  
    PTC_PTC3 = 0;          // en estado 0 la luz de emergencia se apaga
  else  
    PTC_PTC3 = OsciladorEstadoPTC2;  // en estado 1 la luz de emergencia titila
}

void RutinaInicial(void) @ "CODIGO__DEMONIOS" {
}