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
* ARCHIVO: procesos.h                                                        *
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

#ifndef _PROCESOS_H_
#define _PROCESOS_H_

#include "hardware.h"
#include "memoria.h"
#include "sistema.h"

/*
******************************************************************************
* 	                     DEFINICIONES DE LOS PROCESOS                        *
******************************************************************************
*/

/* 
  Defino como PROCESO a aquel programa que espera datos del usuario para poder
  ejecutarse, por ejemplo, medidor de gasolina, medidor de bateria, medidor de 
  RPM, etc. Todos ellos toman datos del carro (en este caso) para actualizarlos
  en las variables correspondientes.
  
  Defino como DEMONIO a aquel programa que no utiliza datos del mundo exterior 
  para ejecutarse, como el controlador de tablero o el oscilador de frenos.
  
  La diferencias operativas entre ellos es que los demonios utilizan la RAM del 
  SO mientras que los procesos tienen su propia RAM asignada, los demonios pue-
  den tener acceso a variables del SO y los demonios se ejecutan uno detras del
  otro en el despachador antes de saltar al siguiente proceso.
  
  Jerarquía del sistema, mientras mas abajo mas privilegios  
  
  *--------------------*
  |                    |
  |      PROCESOS      |
  |                    |
  *--------------------*
  |                    |
  |      DEMONIOS      |
  |                    |
  *--------------------*
  |                    |
  |         SO         |
  |                    |
  *--------------------*
  
*/

#define BIT_0       ((1<<0)&0xFF)
#define BIT_1       ((1<<1)&0xFF)
#define BIT_2       ((1<<2)&0xFF)
#define BIT_3       ((1<<3)&0xFF)
#define BIT_4       ((1<<4)&0xFF)
#define BIT_5       ((1<<5)&0xFF)
#define BIT_6       ((1<<6)&0xFF)
#define BIT_7       ((1<<7)&0xFF)

/*
  Tooodo lo que estamos haciendo con este cosito es para poder llenar esta 
  estructura y enviarla por telemetria:
*/

typedef struct s_carro {
  unsigned char Velocidad;
  unsigned int RPM;  
  char Estado; 
} CARRO;       

/* He aqui las constantes necesarias para calcular la velocidad del carro */
#define R           10.5                // Radio de la rueda en pulgadas
#define PERIMETRO   R*1.596E-4          // Perimetro en kilometro de la rueda
#define DELTA       2.417E-9            // Cada cuanto se incrementa el contador del TIM en horas 
#define KVEL        PERIMETRO/DELTA     // Constante usada para calcular la velocidad


/* He aqui las constantes necesarias para calcular las RPM del carro */          
#define TIM1_DIVISOR              64
#define FRECUENCIA_INTERNA_BUS    7.3728E+6
#define TIM1_PERIODO              ((1)/(FRECUENCIA_INTERNA_BUS))*(TIM1_DIVISOR)
#define KRPM                      ((1)/(TIM1_PERIODO/60))
//#define KRPM                      6912000
        
/* 
  Esta es la configuracion del byte Estado de la estructura CARRO
  
      b7     b6     b5     b4     b3     b2     b1     b0   
   | BAT2 | BAT1 | BAT0 | GAS2 | GAS1 | GAS0 | FRNO | XXXX |
   |       Bateria      |      Gasolina      |Frenos| LIBRE|
   
Bateria Nivel 4 =     1 0 0
Bateria Nivel 3 =     0 1 1
Bateria Nivel 2 =     0 1 0
Bateria Nivel 1 =     0 0 1
Bateria Nivel 0 =     0 0 0

Gasolina Nivel 4 =    1 0 0
Gasolina Nivel 3 =    0 1 1
Gasolina Nivel 2 =    0 1 0
Gasolina Nivel 1 =    0 0 1
Gasolina Nivel 0 =    0 0 0

Frenos Aplicados =    1
Frenos NoAplicados =  0

Funcionamiento Automatico =   1
Funcionamiento Manual =       0
*/

/* Estas definiciones son para enmascarar los bits en CARRO.Estado */
#define CARRO_ESTADO_BIT_BAT2         ((1<<7)&0xFF)
#define CARRO_ESTADO_BIT_BAT1         ((1<<6)&0xFF)
#define CARRO_ESTADO_BIT_BAT0         ((1<<5)&0xFF)
#define CARRO_ESTADO_BIT_GAS2         ((1<<4)&0xFF)
#define CARRO_ESTADO_BIT_GAS1         ((1<<3)&0xFF)
#define CARRO_ESTADO_BIT_GAS0         ((1<<2)&0xFF)
#define CARRO_ESTADO_BIT_FRNO         ((1<<1)&0xFF)

#define CARRO_ESTADO_MASCARA_BAT      (CARRO_ESTADO_BIT_BAT2|CARRO_ESTADO_BIT_BAT1|CARRO_ESTADO_BIT_BAT0)
#define CARRO_ESTADO_MASCARA_GAS      (CARRO_ESTADO_BIT_GAS2|CARRO_ESTADO_BIT_GAS1|CARRO_ESTADO_BIT_GAS0)

/* Estas definiciones son para escribir en CARRO.Estado */
#define CARRO_BATERIA_N4(i)   (i.Estado = (char)((i.Estado&(~CARRO_ESTADO_MASCARA_BAT))|0x80))
#define CARRO_BATERIA_N3(i)   (i.Estado = (char)((i.Estado&(~CARRO_ESTADO_MASCARA_BAT))|0x60))
#define CARRO_BATERIA_N2(i)   (i.Estado = (char)((i.Estado&(~CARRO_ESTADO_MASCARA_BAT))|0x40))
#define CARRO_BATERIA_N1(i)   (i.Estado = (char)((i.Estado&(~CARRO_ESTADO_MASCARA_BAT))|0x20))
#define CARRO_BATERIA_N0(i)   (i.Estado = (char)(i.Estado&(~CARRO_ESTADO_MASCARA_BAT)))

/* Estas definiciones son para escribir en CARRO.Estado */
#define CARRO_GASOLINA_N4(i)  (i.Estado = (char)((i.Estado&(~CARRO_ESTADO_MASCARA_GAS))|0x10))
#define CARRO_GASOLINA_N3(i)  (i.Estado = (char)((i.Estado&(~CARRO_ESTADO_MASCARA_GAS))|0x0C))
#define CARRO_GASOLINA_N2(i)  (i.Estado = (char)((i.Estado&(~CARRO_ESTADO_MASCARA_GAS))|0x08))
#define CARRO_GASOLINA_N1(i)  (i.Estado = (char)((i.Estado&(~CARRO_ESTADO_MASCARA_GAS))|0x04))
#define CARRO_GASOLINA_N0(i)  (i.Estado = (char)(i.Estado&(~CARRO_ESTADO_MASCARA_GAS)))

/* Estas definiciones son para escribir en CARRO.Estado */
#define CARRO_FRENOS_ACTIVADOS                  (i.Estado&CARRO_ESTADO_BIT_FRNO)
#define CARRO_DECLARAR_FRENOS_ACTIVADOS(i)      (i.Estado |= CARRO_ESTADO_BIT_FRNO)
#define CARRO_DECLARAR_FRENOS_NO_ACTIVADOS(i)   (i.Estado &= ~CARRO_ESTADO_BIT_FRNO)

/* Estas definiciones son para escribir en CARRO.Estado */
#define CARRO_MODO_AUTO                         (PTC_PTC0)

/*
******************************************************************************
* 	                     DECLARACIONES DE LOS PROCESOS                       *
******************************************************************************
*/
    
void VerificarFrenos(void) @ "CODIGO__P0";
void MedirBateria(void) @ "CODIGO__P1";
void MedirGasolina(void) @ "CODIGO__P2";
void MedirVelocidad(void) @ "CODIGO__P3";
void MedirRPM(void) @ "CODIGO__P4";
void EnviarPaquete(void) @ "CODIGO__P5";
void CrearNibbles(void) @ "CODIGO__P5";   
void CrearHex(void) @ "CODIGO__P5";
void CrearCodificado(void) @ "CODIGO__P5";

void ControlarTablero(void) @ "CODIGO__DEMONIOS";
void OsciladorFrenos(void) @ "CODIGO__DEMONIOS";
void RutinaInicial(void) @ "CODIGO__DEMONIOS";

#endif // _PROCESOS_H_