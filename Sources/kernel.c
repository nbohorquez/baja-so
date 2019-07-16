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
* ARCHIVO: kernel.c                                                          *
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
#include "kernel.h"

/*
******************************************************************************
*			                   DEFINICIONES DE PROCESOS Y COLAS                    *
******************************************************************************
*/

/* 
   Aqui estan definido todas las Cedulas de los procesos y demonios que 
   seran ejecutados por el microcontrolador
*/

#define NUMERO_DE_PROCESOS  6
#define NUMERO_DE_DEMONIOS  2

#define VERIFICAR_FRENOS    0x00
#define MEDIR_BATERIA       0x01
#define MEDIR_GASOLINA      0x02
#define MEDIR_VELOCIDAD     0x03
#define MEDIR_RPM           0x04
#define ENVIAR_PAQUETE      0x05

#define CONTROLAR_TABLERO   0xFF
#define OSCILADOR_FRENOS    0xFE

BCP *Temporal = (BCP *)NULL;
BCP Proceso[NUMERO_DE_PROCESOS] = {
  // Proceso[0]
  { VERIFICAR_FRENOS,             // Cedula
    BCP_BLOQUEADO_IRQ, 0,         // Estado y ContadorSueno
    ((PROC0__RAM-5)>>8)&(0x00FF), // SP_H... el -5s es porque quiero que ya tenga algo en la pila
    ((PROC0__RAM-5)&0x00FF),      // SP_L
    (BCP *)NULL, &Proceso[3]      // Anterior y Siguiente proceso en la cola respectivamente.
  }, 
  // Proceso[1]
  { MEDIR_BATERIA,                // Cedula
    BCP_LISTO, 0,                 // Estado y ContadorSueno
    ((PROC1__RAM-5)>>8)&(0x00FF), // SP_H
    ((PROC1__RAM-5)&0x00FF),      // SP_L
    &Proceso[5], &Proceso[2]      // Anterior y Siguiente proceso en la cola respectivamente.
  },
  // Proceso[2]
  { MEDIR_GASOLINA,               // Cedula
    BCP_LISTO, 0,                 // Estado y ContadorSueno
    ((PROC2__RAM-5)>>8)&(0x00FF), // SP_H... el -5s es porque quiero que ya tenga algo en la pila
    ((PROC2__RAM-5)&0x00FF),      // SP_L
    &Proceso[1], &Proceso[5]      // Anterior y Siguiente proceso en la cola respectivamente.
  }, 
  // Proceso[3]
  { MEDIR_VELOCIDAD,              // Cedula
    BCP_BLOQUEADO_TIM2, 0,        // Estado y ContadorSueno
    ((PROC3__RAM-5)>>8)&(0x00FF), // SP_H
    ((PROC3__RAM-5)&0x00FF),      // SP_L
    &Proceso[0], &Proceso[4]      // Anterior y Siguiente proceso en la cola respectivamente.
  },
  // Proceso[4]
  { MEDIR_RPM,                    // Cedula
    BCP_BLOQUEADO_TIM1, 0,        // Estado y ContadorSueno
    ((PROC4__RAM-5)>>8)&(0x00FF), // SP_H
    ((PROC4__RAM-5)&0x00FF),      // SP_L
    &Proceso[3], (BCP *)NULL      // Anterior y Siguiente proceso en la cola respectivamente.
  },
  // Proceso[5]
  { ENVIAR_PAQUETE,               // Cedula
    BCP_LISTO, 0,                 // Estado y ContadorSueno
    ((PROC5__RAM-5)>>8)&(0x00FF), // SP_H... el -5s es porque quiero que ya tenga algo en la pila
    ((PROC5__RAM-5)&0x00FF),      // SP_L
    &Proceso[2], &Proceso[1]      // Anterior y Siguiente proceso en la cola respectivamente.
  }
};

LISTOS ColaDeListos = {         // Procesos elegibles para ser ejecutados
  // *<-->Proceso[1]<-->Proceso[2]<-->Proceso[5]<-->*
  &Proceso[5], &Proceso[1], &Proceso[5]
  
};

BLOQUEADOS ColaDeBloqueados = { // Procesos que no se pueden ejecutar por falta de recursos
  // NULL-->Proceso[0]<-->Proceso[3]<-->Proceso[4]-->NULL
  &Proceso[0], &Proceso[4]
};
  
ENESPERA ColaDeEspera = {       // Procesos dormidos por el temporizador
  (BCP *)NULL, (BCP *)NULL
};

// Estos son los demonios que voy a llamar
extern void ControlarTablero(void);
extern void OsciladorFrenos(void);
extern void RutinaInicial(void);

// Este es un arreglo de apuntadores a Demonios
void (*Demonios[NUMERO_DE_DEMONIOS])() = { 
  ControlarTablero, 
  OsciladorFrenos 
};

/*
******************************************************************************
*                                  VARIABLES                                 *
******************************************************************************
*/
extern MCU Micro;

char Llamada;
unsigned char TempByteA;                       // 1 Byte
unsigned int TempWordA, TempWordB, TempWordC;  // 2 Bytes

/*
******************************************************************************
*                              CONTROL DE PROCESOS                           *
******************************************************************************
*/

void main(void) {
  MCU_init();         // Se inicializa todo el hardware necesario
  RutinaInicial();    // Este es el famoso juego de luces que dice el Maita...
  __asm jmp Despachador
}

void Despachador(void) {
  // Ejecuta primero los demonios
  for(TempByteA = 0; TempByteA < NUMERO_DE_DEMONIOS; TempByteA++)
    (*Demonios[TempByteA])();
  
  // El Despachador se queda en un ciclo infinito hasta encontrar un proceso listo
  for(Temporal = ColaDeListos.EnEjecucion->Siguiente;;Temporal = Temporal->Siguiente)
    if(Temporal->Estado == BCP_LISTO)
      break;
    
  ColaDeListos.EnEjecucion = Temporal;
  TempWordA = ((ColaDeListos.EnEjecucion->SP_H)<<8)&(0xFF00);
  TempWordA |= (ColaDeListos.EnEjecucion->SP_L)&(0xFF);
  
  asm {
    ldhx TempWordA
    txs         ; // Transferimos ahora a SP
    pulh        ; // Sacamos de una vez a H
    bset 1,TBCR ; // Prendemos el cosito
    rti         ; // Le damos play
  }
}

void ActivarDesactivarProceso(char TipoDeAccion, char TipoDeCola, char Cedula) {
  asm {
    psha
    tpa
    sta TempByteA
    pula
  }
  DisableInterrupts;
  
  switch(TipoDeAccion) {
    case ACTIVAR_PROCESO:
      if(TipoDeCola == COLA_DE_BLQDOS)
        Temporal = BLOQUEADOS_SacarProceso(Cedula);
      else if(TipoDeCola == COLA_DE_ESPERA)
        Temporal = ENESPERA_SacarProceso(Cedula);
      
      if(Temporal != (BCP *)NULL)
        LISTOS_MeterProceso(Temporal);
      break;
    case DESACTIVAR_PROCESO:
      if(Cedula == ColaDeListos.EnEjecucion->Cedula)
        ColaDeListos.EnEjecucion = ColaDeListos.EnEjecucion->Anterior;
      
      Temporal = LISTOS_SacarProceso(Cedula);
      if(TipoDeCola == COLA_DE_BLQDOS)
        BLOQUEADOS_MeterProceso(Temporal);
      else if(TipoDeCola == COLA_DE_ESPERA)
        ENESPERA_MeterProceso(Temporal);
      break;
    default:
      break;
  }
  asm {
    psha
    lda TempByteA
    tap
    pula
  }
}

void EncolarProceso(char TipoDeCola, BCP *Bloque) {
  BLOQUEADOS *Cola = (BLOQUEADOS *)NULL;
  
  switch(TipoDeCola) {
    case COLA_DE_LISTOS:
      Cola = (BLOQUEADOS *)&ColaDeListos.Primero;
      Cola->Ultimo->Siguiente = Bloque; // Lo metemos al final de la cola de listos
      Cola->Primero->Anterior = Bloque;
      Bloque->Anterior = Cola->Ultimo;
      Bloque->Siguiente = Cola->Primero;
      Cola->Ultimo = Bloque;
      Bloque->Estado = BCP_LISTO;
      break;
    case COLA_DE_BLQDOS:
      Cola = &ColaDeBloqueados;
      goto ComunBLQDOS_ESPERA;
    case COLA_DE_ESPERA:
      Cola = (BLOQUEADOS *)&ColaDeEspera;
      Bloque->Estado = BCP_ESPERA;

ComunBLQDOS_ESPERA:      
      if(Cola->Primero != (BCP *)NULL) { // Si hay por lo menos un proceso en la cola de bloqueados
        Cola->Ultimo->Siguiente = Bloque;
        Bloque->Anterior = Cola->Ultimo;
      }
      else { // Sino entonces es el primero (y el ultimo tambien)
        Cola->Primero = Bloque;
        Cola->Primero->Anterior = (BCP *)NULL;
      }        
      Bloque->Siguiente = (BCP *)NULL;
      Cola->Ultimo = Bloque;
      break;
    default:
      break;
  }
}

BCP *DesencolarProceso(char TipoDeCola, char Cedula) {
  BCP *Temporal;
  BLOQUEADOS *Cola = (BLOQUEADOS *)NULL;

  /* 
    Este codigo tiene la intencion de utilizar un solo apuntador 
    para direccionar cualesquiera de los tres tipos de TAD, pero 
    existe un inconveniente: LISTOS es un tipo de TAD distinto a 
    BLOQUEADOS o ESPERA debido a diferencias de tamaño con estos 
    dos ultimos. Para ello se engaña al programa diciendole que 
    la estructura LISTOS comienza no con un apuntador a 
    BCP *EnEjecucion, sino a BCP *Primero, de esta forma los 
    tamaños se igualan y podemos utilizar un apuntador universal.
    
    He aqui un cosito pa' que me entendais (la flechita es el apuntador):
    
    typedef struct s_listos {
      BCP *EnEjecucion;
  --->BCP *Primero;
      BCP *Ultimo;
    } LISTOS;
    
    typedef struct s_bloqueados {
  --->BCP *Primero;
      BCP *Ultimo;
    } BLOQUEADOS;
    
    typedef struct s_espera {
  --->BCP *Primero;
      BCP *Ultimo;
    } ESPERA;
  */
   
  switch(TipoDeCola) {
    case COLA_DE_LISTOS:
      Cola = (BLOQUEADOS *)&ColaDeListos.Primero;
      break;
    case COLA_DE_BLQDOS:
      Cola = &ColaDeBloqueados;
      break;
    case COLA_DE_ESPERA:
      Cola = (BLOQUEADOS *)&ColaDeEspera;
      break;
    default:
      break;
  }
  
  if(Cola->Primero == (BCP *)NULL) { // Si no hay procesos en la cola, no tiene sentido lo que sigue
    Temporal = (BCP *) NULL;
    goto FinDESENCOLAR;
  }
  
  if(TipoDeCola == COLA_DE_LISTOS) {
    Temporal = Cola->Primero;
    do {
      if(Temporal->Cedula == Cedula) {
        // Lo sacamos de la cola
        Temporal->Anterior->Siguiente = Temporal->Siguiente;
        Temporal->Siguiente->Anterior = Temporal->Anterior;
      
        // Hacemos esto para evitar posibles conflictos
        if(Temporal == ColaDeListos.Primero && Temporal == ColaDeListos.Ultimo)
          Cola->Primero = Cola->Ultimo = (BCP *)NULL;
        else if(Temporal == ColaDeListos.Ultimo && Temporal != ColaDeListos.Primero) 
          Cola->Ultimo = Temporal->Anterior;
        else if(Temporal == ColaDeListos.Primero && Temporal != ColaDeListos.Ultimo)
          Cola->Primero = Temporal->Siguiente;
        break;
      }
    Temporal = Temporal->Siguiente;
    } while(Temporal != Cola->Primero);
  } 
  else if(TipoDeCola == COLA_DE_BLQDOS || TipoDeCola == COLA_DE_ESPERA) {
    for(Temporal = Cola->Primero; Temporal != Cola->Ultimo->Siguiente; Temporal = Temporal->Siguiente) {
      if(Temporal->Cedula == Cedula) {
        if(Cola->Ultimo == Cola->Primero)          // Hay un solo proceso bloqueado
          Cola->Ultimo = Cola->Primero = (BCP *)NULL;
        
        else if(Temporal == Cola->Ultimo) {        // Soy ultimo
          Cola->Ultimo = Temporal->Anterior;
          Cola->Ultimo->Siguiente = (BCP *)NULL;
        } 
        else if(Temporal == Cola->Primero) {       // Soy primero
          Cola->Primero = Temporal->Siguiente;
          Cola->Primero->Anterior = (BCP *)NULL;
        } 
        else {
          Temporal->Anterior->Siguiente = Temporal->Siguiente;
          Temporal->Siguiente->Anterior = Temporal->Anterior;
        }
        break;
      }
    }
  }

FinDESENCOLAR:
  return(Temporal);
}  

char Sistema(char CodigoLlamada) {
  asm {
    swi           ; // Hacemos interrupcion por software, el argumento esta guardado en ACC
    rts
  }
}  

/*
******************************************************************************
*                              LLAMADAS AL SISTEMA                           *
******************************************************************************
*/

void isrINT_SWI(void) {
  asm {
    tsx
    stx TempWordA:1       ; // Guardamos la parte alta de SP
    pshh
    pulx
    stx TempWordA:0       ; // Guardamos la parte baja de SP
    sta Llamada           ; // No hace falta cargar el argumento porque ya esta en ACC
    ldhx #KERNEL_RAM      ; // Cargamos la pila del kernel para no abusar de la del programa en ejecucion
    aix #$01
    txs
  }
  
  // Guardamos el SP del proceso que hizo la llamada
  ColaDeListos.EnEjecucion->SP_H = (TempWordA>>8)&0xFF;
  ColaDeListos.EnEjecucion->SP_L = TempWordA&0xFF;
  
  switch(Llamada) {

/*
******************************************************************************
*                            LLAMADAS AL MODULO ADC                          * 
******************************************************************************
*/
    case ADC_LEER_CNL0:
    case ADC_LEER_CNL1:
    case ADC_LEER_CNL2:
    case ADC_LEER_CNL3:
      break;
    case ADC_LEER_CNL4:
      // El proceso se bloquea porque tiene que esperar al ADC
      ColaDeListos.EnEjecucion->Estado = BCP_BLOQUEADO_ADC;
      Bloquear(ColaDeListos.EnEjecucion->Cedula);
      
      // Aumentamos el numero de clientes del modulo ADC
      Micro.ModuloADC.Clientes++;
            
      // Si el modulo ADC no esta ocupado entonces lo manda a trabajar 
      if(!(ADC_OCUPADO(Micro.ModuloADC))) { 
        ADC_OCUPAR(Micro.ModuloADC);
        ADC_PTB4_AD4;
      }
      break;
    case ADC_LEER_CNL5:
    case ADC_LEER_CNL6:
    case ADC_LEER_CNL7:
      break;
      
/*
******************************************************************************
*                            LLAMADAS AL MODULO SCI                          * 
******************************************************************************
*/
    case SCI_ESTABLECER_BUFFER:
      // El buffer del SCI esta contenido en H:X del proceso llamante
      TempWordA = ((ColaDeListos.EnEjecucion->SP_H)<<8)&(0xFF00);
      TempWordA |= (ColaDeListos.EnEjecucion->SP_L)&(0xFF);

      asm {
        tsx
        stx TempWordB:1
        pshh
        pulx
        stx TempWordB:0
        
        ldhx TempWordA
        txs             ; // Cargamos el SP del proceso cliente
        lda PILA_H,SP   ; // Cargamos H. Contiene la parte baja de la direccion del buffer
        sta TempWordC:0
        lda PILA_X,SP   ; // Cargamos X. Contiene la parte alta de la direccion del buffer
        sta TempWordC:1
        
        ldx TempWordB:0
        pshx
        pulh
        ldx TempWordB:1
        txs
      }
      Micro.ModuloSCI.Buffer = (char *)TempWordC;
      Micro.ModuloSCI.TamanoBuffer = *(Micro.ModuloSCI.Buffer);
      break;
/*
******************************************************************************
*                            LLAMADAS AL MODULO IRQ                          * 
******************************************************************************
*/
    case IRQ_BLOQUEARSE:
      ColaDeListos.EnEjecucion->Estado = BCP_BLOQUEADO_IRQ;
      Bloquear(ColaDeListos.EnEjecucion->Cedula);
      Micro.ModuloIRQ.Clientes++;
      break;
/*
******************************************************************************
*                            LLAMADAS AL MODULO TIM1                         * 
******************************************************************************
*/
    case TIM1_BLOQUEARSE:
      ColaDeListos.EnEjecucion->Estado = BCP_BLOQUEADO_TIM1;
      Bloquear(ColaDeListos.EnEjecucion->Cedula);
      Micro.ModuloTIM1.Clientes++;
      break;
      
    case TIM1_CNL0_ESPERAR_PRIMER_PULSO:
      TIM_ESPERAR_PRIMER_PULSO(Micro.ModuloTIM1.CNL0);
      break;
    case TIM1_CNL0_ESPERAR_SEGUNDO_PULSO:
      TIM_ESPERAR_SEGUNDO_PULSO(Micro.ModuloTIM1.CNL0);
      break;
      
    case TIM1_CNL1_ESPERAR_PRIMER_PULSO:
      TIM_ESPERAR_PRIMER_PULSO(Micro.ModuloTIM1.CNL1);
      break;
    case TIM1_CNL1_ESPERAR_SEGUNDO_PULSO:
      TIM_ESPERAR_SEGUNDO_PULSO(Micro.ModuloTIM1.CNL1);
      break;
      
    case TIM1_CNL0_LEER_ESTADO:
      TempByteA = Micro.ModuloTIM1.CNL0.Estado;
      TIM_DECLARAR_TIEMPO_NO_ACTUALIZADO(Micro.ModuloTIM1.CNL0);
      goto Comun_TIM;
    case TIM1_CNL0_LEER_CONTADOR_H:
      TempByteA = Micro.ModuloTIM1.CNL0.Contador_H;
      goto Comun_TIM;
    case TIM1_CNL0_LEER_CONTADOR_L:
      TempByteA = Micro.ModuloTIM1.CNL0.Contador_L;
      goto Comun_TIM;
    
    case TIM1_CNL1_LEER_ESTADO:
      TempByteA = Micro.ModuloTIM1.CNL1.Estado;
      TIM_DECLARAR_TIEMPO_NO_ACTUALIZADO(Micro.ModuloTIM1.CNL1);
      goto Comun_TIM;
    case TIM1_CNL1_LEER_CONTADOR_H:
      TempByteA = Micro.ModuloTIM1.CNL1.Contador_H;
      goto Comun_TIM;
    case TIM1_CNL1_LEER_CONTADOR_L:
      TempByteA = Micro.ModuloTIM1.CNL1.Contador_L;
      goto Comun_TIM;
/*
******************************************************************************
*                            LLAMADAS AL MODULO TIM2                         * 
******************************************************************************
*/
    case TIM2_BLOQUEARSE:
      ColaDeListos.EnEjecucion->Estado = BCP_BLOQUEADO_TIM2;
      Bloquear(ColaDeListos.EnEjecucion->Cedula);
      Micro.ModuloTIM2.Clientes++;
      break;
      
    case TIM2_CNL0_ESPERAR_PRIMER_PULSO:
      TIM_ESPERAR_PRIMER_PULSO(Micro.ModuloTIM2.CNL0);
      break;
    case TIM2_CNL0_ESPERAR_SEGUNDO_PULSO:
      TIM_ESPERAR_SEGUNDO_PULSO(Micro.ModuloTIM2.CNL0);
      break;
      
    case TIM2_CNL1_ESPERAR_PRIMER_PULSO:
      TIM_ESPERAR_PRIMER_PULSO(Micro.ModuloTIM2.CNL1);
      break;
    case TIM2_CNL1_ESPERAR_SEGUNDO_PULSO:
      TIM_ESPERAR_SEGUNDO_PULSO(Micro.ModuloTIM2.CNL1);
      break;
      
    case TIM2_CNL0_LEER_ESTADO:
      TempByteA = Micro.ModuloTIM2.CNL0.Estado;
      TIM_DECLARAR_TIEMPO_NO_ACTUALIZADO(Micro.ModuloTIM2.CNL0);
      goto Comun_TIM;
    case TIM2_CNL0_LEER_CONTADOR_H:
      TempByteA = Micro.ModuloTIM2.CNL0.Contador_H;
      goto Comun_TIM;
    case TIM2_CNL0_LEER_CONTADOR_L:
      TempByteA = Micro.ModuloTIM2.CNL0.Contador_L;
      goto Comun_TIM;
      
    case TIM2_CNL1_LEER_ESTADO:
      TempByteA = Micro.ModuloTIM2.CNL1.Estado;
      TIM_DECLARAR_TIEMPO_NO_ACTUALIZADO(Micro.ModuloTIM2.CNL1);
      goto Comun_TIM;
    case TIM2_CNL1_LEER_CONTADOR_H:
      TempByteA = Micro.ModuloTIM2.CNL1.Contador_H;
      goto Comun_TIM;
    case TIM2_CNL1_LEER_CONTADOR_L:
      TempByteA = Micro.ModuloTIM2.CNL1.Contador_L;
      
Comun_TIM:
      // Vamos a colocar el dato en la pila del proceso
      TempWordA = ((ColaDeListos.EnEjecucion->SP_H)<<8)&(0xFF00);
      TempWordA |= (ColaDeListos.EnEjecucion->SP_L)&(0xFF);

      asm {
        tsx
        stx TempWordB:1
        pshh
        pulx
        stx TempWordB:0
        
        ldhx TempWordA
        txs             ; // Cargamos el SP del proceso cliente
        lda TempByteA
        sta PILA_ACC,SP        
        ldx TempWordB:0
        pshx
        pulh
        ldx TempWordB:1
        txs
      }
      break;
/*
******************************************************************************
*                            LLAMADAS AL DESPACHADOR                         * 
******************************************************************************
*/
    case DESP_REINICIAR:
      ColaDeListos.EnEjecucion = ColaDeListos.Ultimo;
      break;
    case DESP_SIGUIENTE:
    case NADA:
    default:
      break;
  }
  
  /* 
    Hay que hacer esto porque el TBM esta configurado a una 
    velocidad demasiado rapida y no deja que se ejecute el 
    proceso inmediatamente posterior
  */
  if(TBM_PENDIENTE)
    TBM_ACUSAR;
  TBM_APAGAR;
  
  __asm jmp Despachador   ; // Saltamos a Despachador sin hacer RTI para mantener las interrupciones deshabilitadas
}