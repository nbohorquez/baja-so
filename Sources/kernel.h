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
* ARCHIVO: kernel.h                                                          *
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

#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "hardware.h"
#include "memoria.h"
#include "sistema.h"

/*
******************************************************************************
* 				                  DEFINICIONES DEL KERNEL                          *
******************************************************************************
*/
 
/* 
   Aqui esta definido todo lo referido al bloque de control de procesos
   y se identifican sus constantes con la nomenclatura BCP_NNNNNN
*/

#define BCP_ESPERA            0     // esta en espera de culminacion del temporizador que lo mando a dormir
#define BCP_LISTO             1     // esta listo para ejecutarse en espera en la cola round robin
#define BCP_DEMONIO           2
#define BCP_BLOQUEADO         3     // esta bloqueado debido a falta de recurso de hardware genérico
#define BCP_BLOQUEADO_ADC     4     // esta bloqueado esperando dato del ADC
#define BCP_BLOQUEADO_IRQ     5     // esta bloqueado esperando evento del IRQ
#define BCP_BLOQUEADO_KBI     6     // esta bloqueado esperando evento del KBI
#define BCP_BLOQUEADO_SCI     7     // esta bloqueado esperando evento del SCI
#define BCP_BLOQUEADO_TIM1    8     // esta bloqueado esperando evento del TIM1
#define BCP_BLOQUEADO_TIM2    9     // esta bloqueado esperando evento del TIM2

typedef struct s_bcp {
  char Cedula;                // identificacion del proceso
  char Estado;                // identificacion del estado del proceso
  unsigned char ContadorSueno;
  char SP_H, SP_L;            // contexto de ejecucion del proceso donde se guardan los registros
  struct s_bcp *Anterior;     // esto es para hacer la cola round robin, espera o bloqueado
  struct s_bcp *Siguiente;    // esto es para hacer la cola round robin, espera o bloqueado
} BCP;

/* 
  Funciones especificas del TAD BCP... estan comentadas porque me parecen muy formales e innecesarias

#define BCP_LeerCedula(i)         (char) ((BCP)i).Cedula
#define BCP_LeerEstado(i)         (char) ((BCP)i).Estado
#define BCP_LeerContadorSueno(i)  (char) ((BCP)i).ContadorSueno
#define BCP_LeerSP_H(i)           (char) ((BCP)i).SP_H
#define BCP_LeerSP_L(i)           (char) ((BCP)i).SP_L
#define BCP_LeerAnterior(i)       (BCP *) ((BCP)i).Anterior
#define BCP_LeerSiguiente(i)      (BCP *) ((BCP)i).Siguiente

#define BCP_EscribirCedula(i,c)         ((BCP)i).Cedula = (char) c
#define BCP_EscribirEstado(i,c)         ((BCP)i).Estado = (char) c
#define BCP_EscribirContadorSueno(i,c)  ((BCP)i).ContadorSueno = (char) c
#define BCP_EscribirSP_H(i,c)           ((BCP)i).SP_H = (char) c
#define BCP_EscribirSP_L(i,c)           ((BCP)i).SP_L = (char) c
#define BCP_EscribirAnterior(i,c)       ((BCP)i).Anterior = (BCP *) c
#define BCP_EscribirSiguiente(i,c)      ((BCP)i).Siguiente = (BCP *) c
*/

typedef struct s_listos {
  BCP *EnEjecucion;           // apunta al proceso en actual ejecucion 
  BCP *Primero;
  BCP *Ultimo;
} LISTOS;

/*
  Funciones especificas del TAD LISTOS... algunas estan comentadas por la misma razon que estan comentadas
  las del TAD BCP
  
#define LISTOS_LeerEnEjecucion(i)   (BCP *) ((LISTOS)i).EnEjecucion
#define LISTOS_LeerPrimero(i)       (BCP *) ((LISTOS)i).Primero
#define LISTOS_LeerUltimo(i)        (BCP *) ((LISTOS)i).Ultimo

#define LISTOS_EscribirEnEjecucion(i,c)     ((LISTOS)i).EnEjecucion = (BCP *) c
#define LISTOS_EscribirPrimero(i,c)         ((LISTOS)i).Primero = (BCP *) c
#define LISTOS_EscribirUltimo(i,c)          ((LISTOS)i).Ultimo = (BCP *) c
*/

typedef struct s_bloqueados {
  BCP *Primero;
  BCP *Ultimo;
} BLOQUEADOS;

/*
  Funciones especificas del TAD BLOQUEADOS... algunas estan comentadas por la misma razon que estan 
  comentadas las del TAD BCP

#define BLOQUEADOS_LeerPrimero(i)       (BCP *) ((BLOQUEADOS)i).Primero
#define BLOQUEADOS_LeerUltimo(i)        (BCP *) ((BLOQUEADOS)i).Ultimo

#define BLOQUEADOS_EscribirPrimero(i,c)         ((BLOQUEADOS)i).Primero = (BCP *) c
#define BLOQUEADOS_EscribirUltimo(i,c)          ((BLOQUEADOS)i).Ultimo = (BCP *) c
*/

typedef struct s_enespera {
  BCP *Primero;
  BCP *Ultimo;
} ENESPERA;

/*
  Funciones especificas del TAD ENESPERA... algunas estan comentadas por la misma razon que estan 
  comentadas las del TAD BCP

#define ENESPERA_LeerPrimero(i)       (BCP *) ((ENESPERA)i).Primero
#define ENESPERA_LeerUltimo(i)        (BCP *) ((ENESPERA)i).Ultimo

#define ENESPERA_EscribirPrimero(i,c)         ((ENESPERA)i).Primero = (BCP *) c
#define ENESPERA_EscribirUltimo(i,c)          ((ENESPERA)i).Ultimo = (BCP *) c
*/

typedef struct s_demonios {
  BCP *Primero;
  BCP *Ultimo;
} DEMONIOS;

/*
  Funciones especificas del TAD DEMONIOS... algunas estan comentadas por la misma razon que estan 
  comentadas las del TAD BCP

#define DEMONIOS_LeerPrimero(i)       (BCP *) ((DEMONIOS)i).Primero
#define DEMONIOS_LeerUltimo(i)        (BCP *) ((DEMONIOS)i).Ultimo

#define DEMONIOS_EscribirPrimero(i,c)         ((DEMONIOS)i).Primero = (BCP *) c
#define DEMONIOS_EscribirUltimo(i,c)          ((DEMONIOS)i).Ultimo = (BCP *) c
*/

#define COLA_DE_BLQDOS        0
#define COLA_DE_LISTOS        1
#define COLA_DE_ESPERA        2

#define ACTIVAR_PROCESO       0
#define DESACTIVAR_PROCESO    1

#define Desbloquear(Cedula)   ActivarDesactivarProceso(ACTIVAR_PROCESO, COLA_DE_BLQDOS, Cedula)
#define Bloquear(Cedula)      ActivarDesactivarProceso(DESACTIVAR_PROCESO, COLA_DE_BLQDOS, Cedula)
#define Despertar(Cedula)     ActivarDesactivarProceso(ACTIVAR_PROCESO, COLA_DE_ESPERA, Cedula)
#define Dormir(Cedula)        ActivarDesactivarProceso(DESACTIVAR_PROCESO, COLA_DE_ESPERA, Cedula)

/* Estas funciones de arriba utilizan una o mas de las listadas a continuacion */

#define LISTOS_SacarProceso(Cedula)       (BCP *) DesencolarProceso(COLA_DE_LISTOS, Cedula)
#define BLOQUEADOS_SacarProceso(Cedula)   (BCP *) DesencolarProceso(COLA_DE_BLQDOS, Cedula)
#define ENESPERA_SacarProceso(Cedula)     (BCP *) DesencolarProceso(COLA_DE_ESPERA, Cedula)
#define LISTOS_MeterProceso(Bloque)       EncolarProceso(COLA_DE_LISTOS, (BCP *)Bloque)
#define BLOQUEADOS_MeterProceso(Bloque)   EncolarProceso(COLA_DE_BLQDOS, (BCP *)Bloque)
#define ENESPERA_MeterProceso(Bloque)     EncolarProceso(COLA_DE_ESPERA, (BCP *)Bloque)

/*
******************************************************************************
* 				                    FUNCIONES DEL KERNEL                           *
******************************************************************************
*/

void MCU_init(void);
void Despachador(void);
char Sistema(char CodigoLlamada);
void ActivarDesactivarProceso(char TipoDeAccion, char TipoDeCola, char Cedula);
BCP *DesencolarProceso(char TipoDeCola, char Cedula);
void EncolarProceso(char TipoDeCola, BCP *Bloque);

#endif /* _KERNEL_H_ */