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
* ARCHIVO: hardware.c                                                        *
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
*                                  VARIABLES                                 *
******************************************************************************
*/

MCU Micro = {
  // ModuloADC
  {  
    0, // Clientes = 0
    (~(ADC_ESTADO_BIT_OCUP|ADC_ESTADO_BIT_DATO)) 
  }, 
  // ModuloIRQ  
  { 
    1, // Clientes = VerificarFrenos
    0  // Estado = NoDefinido
  },
  // ModuloSCI
  {
    (char *)NULL, // Buffer apuntando a ninguna parte
    0, 0          // TamañoBuffer = Indice = 0
  },
  // ModuloTIM1
  {
    1,           // Clientes = MedirRPM
    { 0, 0, 0 }, // CNL0 Estado = NoTiempo|EsperandoPrimerPulso; Contador = 0
    { 0, 0, 0 }  // CNL1 Estado = NoTiempo|EsperandoPrimerPulso; Contador = 0
  }, 
  // ModuloTIM2
  { 
    1,           // Clientes = MedirVelocidad
    { 0, 0, 0 }, // CNL0 Estado = NoTiempo|EsperandoPrimerPulso; Contador = 0
    { 0, 0, 0 }  // CNL1 Estado = NoTiempo|EsperandoPrimerPulso; Contador = 0
  }  
};

extern BCP *Temporal;
extern LISTOS ColaDeListos;
extern BLOQUEADOS ColaDeBloqueados;
extern ENESPERA ColaDeEspera;

extern unsigned char TempByteA;                       // 1 Byte
extern unsigned int TempWordA, TempWordB, TempWordC;  // 2 Bytes

/*
******************************************************************************
*                            RUTINAS DE INTERRUPCIONES                       *
******************************************************************************
*/

/*
    INTERRUPCIONES
    ==============
    ----------EstadoDeFrenos ============== isrINT_IRQ
    ----------InterrupciónVelocimetro ===== isrINT_TIM1Ovr y isrINT_KBD
    ----------InterrupciónRPM ============= isrINT_TIM1Ovr y isrINT_TIM1CH0
    ----------DatoEnviado ================= isrINT_SCITransmit
    ----------InterrupciónADC ============= isrINT_ADC
    ----------Llamada al sistema ========== isrINT_SWI
    ----------InterrupcionTBM ============= isrINT_TBM
    
    Int.no.  0 INT_TBM (at FFDC)               Used 
    Int.no.  1 INT_ADC (at FFDE)               Used 
    Int.no.  2 INT_KBD (at FFE0)               Used
    Int.no.  3 INT_SCITransmit (at FFE2)       Used
    Int.no.  4 INT_SCIReceive (at FFE4)        Used
    Int.no.  5 INT_SCIError (at FFE6)          Used
    Int.no.  6 INT_SPITransmit (at FFE8)       Unassigned
    Int.no.  7 INT_SPIReceive (at FFEA)        Unassigned
    Int.no.  8 INT_TIM2Ovr (at FFEC)           Used
    Int.no.  9 INT_TIM2CH1 (at FFEE)           Unassigned
    Int.no. 10 INT_TIM2CH0 (at FFF0)           Unassigned
    Int.no. 11 INT_TIM1Ovr (at FFF2)           Used
    Int.no. 12 INT_TIM1CH1 (at FFF4)           Unassigned
    Int.no. 13 INT_TIM1CH0 (at FFF6)           Used
    Int.no. 14 INT_PLL (at FFF8)               Unassigned
    Int.no. 15 INT_IRQ (at FFFA)               Used
    Int.no. 16 INT_SWI (at FFFC)               Used
    Int.no. 17 INT_RESET (at FFFE)             Reset vector
*/

__interrupt void isrINT_TBM(void) {
/*
  Esta rutina de interrupcion guarda el contexto del programa en ejecucion,
  decrementa el contador de sueño de los procesos dormidos y despierta a 
  aquellos cuyo contador llegue a cero. Luego salta al despachador para elegir
  el siguiente proceso a ejecutar
*/
  
  TBM_ACUSAR;
  TBM_APAGAR;
  asm {
    tsx
    stx TempWordA:1      ; // Guardamos la parte baja de SP
    pshh
    pulx
    stx TempWordA:0      ; // Guardamos la parte alta de SP
    ldhx #KERNEL_RAM     ; // Cargamos la pila del kernel para no abusar de la del programa en ejecucion
    aix #$01
    txs                  
  }

  // Guardamos el SP del proceso interrumpido
  ColaDeListos.EnEjecucion->SP_H = (TempWordA>>8)&0xFF;
  ColaDeListos.EnEjecucion->SP_L = TempWordA&0xFF;
  
  // Verificamos si hay procesos dormidos y disminuimos su(s) contador(es) de sueno en caso de haber alguno
  if(ColaDeEspera.Primero != (BCP *)NULL) {
    for(Temporal = ColaDeEspera.Primero; Temporal != ColaDeEspera.Ultimo->Siguiente; Temporal = Temporal->Siguiente) {
      --(Temporal->ContadorSueno);
      if(!(Temporal->ContadorSueno))    // Se le acabo su tiempo de espera y entra de ultimo al roundrobin
        Despertar(Temporal->Cedula);
    }
  }

  // Saltamos a Despachador sin hacer RTI para mantener las interrupciones deshabilitadas
  __asm jmp Despachador 
}

__interrupt void isrINT_ADC(void) {
/*
  Esta rutina de interrupcion verifica si hay procesos bloqueados por espera de
  dato de ADC. Si efectivamente hay, al primero de ellos se le entrega el dato 
  recien convertido y se desbloquea; si todavia queda un proceso en espera de ADC, 
  se ordena una nueva conversion; si no hay mas, se manda a dormir al ADC.
*/
  asm {
    tsx
    stx TempWordC:1      ; // Guardamos la parte baja de SP
    pshh
    pulx
    stx TempWordC:0      ; // Guardamos la parte alta de SP
    ldhx #KERNEL_RAM     ; // Cargamos la pila del kernel para no abusar de la del programa en ejecucion
    aix #$01
    txs                  
  }

  for(Temporal = ColaDeBloqueados.Primero; Temporal != ColaDeBloqueados.Ultimo->Siguiente; Temporal = Temporal->Siguiente) {
    if(Micro.ModuloADC.Clientes) {
      if(Temporal->Estado == BCP_BLOQUEADO_ADC) {
      // Verificamos si el dato convertido no se ha entregado a nadie todavia
        if(!ADC_DATO_ENTREGADO(Micro.ModuloADC)) {
          ADC_DECLARAR_DATO_ENTREGADO(Micro.ModuloADC);
          Micro.ModuloADC.Clientes--;
          Desbloquear(Temporal->Cedula);
        
          // Vamos a colocar el dato en la pila del proceso
          TempWordA = ((Temporal->SP_H)<<8)&(0xFF00);
          TempWordA |= (Temporal->SP_L)&(0xFF);
          TempByteA = ADR;
  
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
          continue;
        } 
        // Si hay mas procesos bloqueados por ADC, se pone a trabajar el ADC
        else {
          ADC_PTB4_AD4;
          ADC_DECLARAR_DATO_NO_ENTREGADO(Micro.ModuloADC);
          break;
        }
      }
    } 
    // No hay mas procesos esperando por ADC... se apaga entonces
    else {
      ADC_APAGAR;
      ADC_DECLARAR_DATO_NO_ENTREGADO(Micro.ModuloADC);
      // Esto es para que otro proceso pueda venir a usarlo
      ADC_LIBERAR(Micro.ModuloADC);
      break;
    }    
  }
  
  asm {
    ldx TempWordC:0      ; // Cargamos la parte alta de SP
    pshx
    pulh
    ldx TempWordC:1      ; // Guardamos la parte alta de SP
    txs                  ; // Regresamos a la pila que estabamos usando
  }

  /* 
    Hay que hacer esto porque el TBM esta configurado a una 
    velocidad demasiado rapida y no deja que se ejecute el 
    proceso inmediatamente posterior
  */
  if(TBM_PENDIENTE)
    TBM_ACUSAR;
}

__interrupt void isrINT_KBD(void) {
  // Verificamos si este pulso es el primero de dos necesarios para tomar el tiempo
  if(TIM_ESPERANDO_PRIMER_PULSO(Micro.ModuloTIM2.CNL0)) {
    TIM2_DETENER;
    TIM2_VOLVER_A_CERO;
    TIM2_INICIAR;

    // Informamos que ya se tomo el primer pulso
    TIM_ESPERAR_SEGUNDO_PULSO(Micro.ModuloTIM2.CNL0);
  }
  else {
    // Guardamos el contador
    Micro.ModuloTIM2.CNL0.Contador_H = T2CNTH;
    Micro.ModuloTIM2.CNL0.Contador_L = T2CNTL;
    TIM_DECLARAR_TIEMPO_ACTUALIZADO(Micro.ModuloTIM2.CNL0);
    
    asm {
      tsx
      stx TempWordC:1      ; // Guardamos la parte baja de SP
      pshh
      pulx
      stx TempWordC:0      ; // Guardamos la parte alta de SP
      ldhx #KERNEL_RAM     ; // Cargamos la pila del kernel para no abusar de la del programa en ejecucion
      aix #$01
      txs                  
    }

    for(Temporal = ColaDeBloqueados.Primero; Temporal != ColaDeBloqueados.Ultimo->Siguiente && Micro.ModuloTIM2.Clientes; Temporal = Temporal->Siguiente) {
      if(Temporal->Estado == BCP_BLOQUEADO_TIM2) {
        Micro.ModuloTIM2.Clientes--;
        Desbloquear(Temporal->Cedula);
      }
    }
  
    asm {
      ldx TempWordC:0      ; // Cargamos la parte alta de SP
      pshx
      pulh
      ldx TempWordC:1      ; // Guardamos la parte alta de SP
      txs                  ; // Regresamos a la pila que estabamos usando
    }
    
    // Esperamos el primer pulso nuevamente
    TIM_ESPERAR_PRIMER_PULSO(Micro.ModuloTIM2.CNL0);
  }
  TIM2_CNL0_NO_ENT_CAPTR;
  /* 
    Hay que hacer esto porque el TBM esta configurado a una 
    velocidad demasiado rapida y no deja que se ejecute el 
    proceso inmediatamente posterior
  */ 
  if(TBM_PENDIENTE)
    TBM_ACUSAR;
}

__interrupt void isrINT_SCITransmit(void) {
  TempByteA = SCS1;
  
  SCDR = *(Micro.ModuloSCI.Buffer+Micro.ModuloSCI.Indice);
 
  if(Micro.ModuloSCI.Indice == (Micro.ModuloSCI.TamanoBuffer)-1)
    Micro.ModuloSCI.Indice = 0;
  else
    (Micro.ModuloSCI.Indice)++;
   
  /* 
    Hay que hacer esto porque el TBM esta configurado a una 
    velocidad demasiado rapida y no deja que se ejecute el 
    proceso inmediatamente posterior
  */
  if(TBM_PENDIENTE)
    TBM_ACUSAR;
}

__interrupt void isrINT_TIM2Ovr(void) {
/*
  Esta rutina de interrupcion despiera a todos los proceso que esten 
  esperando el evento Desborde de TIM2
*/
  asm {
    tsx
    stx TempWordC:1      ; // Guardamos la parte baja de SP
    pshh
    pulx
    stx TempWordC:0      ; // Guardamos la parte alta de SP
    ldhx #KERNEL_RAM     ; // Cargamos la pila del kernel para no abusar de la del programa en ejecucion
    aix #$01
    txs                  
  }

  TIM2_BORRAR_DESBORDE;
  for(Temporal = ColaDeBloqueados.Primero; Temporal != ColaDeBloqueados.Ultimo->Siguiente && Micro.ModuloTIM2.Clientes; Temporal = Temporal->Siguiente) {
    if(Temporal->Estado == BCP_BLOQUEADO_TIM2) {
      Micro.ModuloTIM2.Clientes--;
      Desbloquear(Temporal->Cedula);
    }
  }
  
  asm {
    ldx TempWordC:0      ; // Cargamos la parte alta de SP
    pshx
    pulh
    ldx TempWordC:1      ; // Guardamos la parte alta de SP
    txs                  ; // Regresamos a la pila que estabamos usando
  }

  /* 
    Hay que hacer esto porque el TBM esta configurado a una 
    velocidad demasiado rapida y no deja que se ejecute el 
    proceso inmediatamente posterior
  */
  if(TBM_PENDIENTE)
    TBM_ACUSAR;
}
 
__interrupt void isrINT_TIM1Ovr(void) {
/*
  Esta rutina de interrupcion despiera a todos los proceso que esten 
  esperando el evento Desborde de TIM1
*/
  asm {
    tsx
    stx TempWordC:1      ; // Guardamos la parte alta de SP
    pshh
    pulx
    stx TempWordC:0      ; // Guardamos la parte baja de SP
    ldhx #KERNEL_RAM     ; // Cargamos la pila del kernel para no abusar de la del programa en ejecucion
    aix #$01
    txs                  
  }

  TIM1_BORRAR_DESBORDE;
  for(Temporal = ColaDeBloqueados.Primero; Temporal != ColaDeBloqueados.Ultimo->Siguiente && Micro.ModuloTIM1.Clientes; Temporal = Temporal->Siguiente) {
    if(Temporal->Estado == BCP_BLOQUEADO_TIM1) {
      Micro.ModuloTIM1.Clientes--;
      Desbloquear(Temporal->Cedula);
    }
  }
  
  asm {
    ldx TempWordC:0      ; // Cargamos la parte baja de SP
    pshx
    pulh
    ldx TempWordC:1      ; // Guardamos la parte alta de SP
    txs                  ; // Regresamos a la pila que estabamos usando
  }

  /* 
    Hay que hacer esto porque el TBM esta configurado a una 
    velocidad demasiado rapida y no deja que se ejecute el 
    proceso inmediatamente posterior
  */
  if(TBM_PENDIENTE)
    TBM_ACUSAR;
}
                                           
__interrupt void isrINT_TIM1CH1(void) {
  // Verificamos si este pulso es el primero de dos necesarios para tomar el tiempo
  if(TIM_ESPERANDO_PRIMER_PULSO(Micro.ModuloTIM1.CNL1)) {
    TIM1_DETENER;
    TIM1_VOLVER_A_CERO;
    TIM1_INICIAR;

    // Informamos que ya se tomo el primer pulso
    TIM_ESPERAR_SEGUNDO_PULSO(Micro.ModuloTIM1.CNL1);
  } 
  else {
  // Guardamos el contador
    Micro.ModuloTIM1.CNL1.Contador_H = T1CH1H;
    Micro.ModuloTIM1.CNL1.Contador_L = T1CH1L;
    TIM_DECLARAR_TIEMPO_ACTUALIZADO(Micro.ModuloTIM1.CNL1);
    
    asm {
      tsx
      stx TempWordC:1      ; // Guardamos la parte baja de SP
      pshh
      pulx
      stx TempWordC:0      ; // Guardamos la parte alta de SP
      ldhx #KERNEL_RAM     ; // Cargamos la pila del kernel para no abusar de la del programa en ejecucion
      aix #$01
      txs                  
    }

    for(Temporal = ColaDeBloqueados.Primero; Temporal != ColaDeBloqueados.Ultimo->Siguiente && Micro.ModuloTIM1.Clientes; Temporal = Temporal->Siguiente) {
      if(Temporal->Estado == BCP_BLOQUEADO_TIM1) {
        Micro.ModuloTIM1.Clientes--;
        Desbloquear(Temporal->Cedula);
      }
    }
  
    asm {
      ldx TempWordC:0      ; // Cargamos la parte alta de SP
      pshx
      pulh
      ldx TempWordC:1      ; // Guardamos la parte alta de SP
      txs                  ; // Regresamos a la pila que estabamos usando
    }

    // Esperamos el primer pulso nuevamente
    TIM_ESPERAR_PRIMER_PULSO(Micro.ModuloTIM1.CNL1);
  }
  TIM1_CNL1_NO_ENT_CAPTR;
  /* 
    Hay que hacer esto porque el TBM esta configurado a una 
    velocidad demasiado rapida y no deja que se ejecute el 
    proceso inmediatamente posterior
  */       
  if(TBM_PENDIENTE)
    TBM_ACUSAR;
}


__interrupt void isrINT_IRQ(void) {
/*
  Esta rutina de interrupcion despiera a todos los proceso que esten 
  esperando el evento IRQ
*/

  IRQ_ACUSAR;
  IRQ_ENMASCARAR;
  
  asm {
    tsx
    stx TempWordC:1      ; // Guardamos la parte baja de SP
    pshh
    pulx
    stx TempWordC:0      ; // Guardamos la parte alta de SP
    ldhx #KERNEL_RAM     ; // Cargamos la pila del kernel para no abusar de la del programa en ejecucion
    aix #$01
    txs                  
  }

  for(Temporal = ColaDeBloqueados.Primero; Temporal != ColaDeBloqueados.Ultimo->Siguiente && Micro.ModuloIRQ.Clientes; Temporal = Temporal->Siguiente) {
    if(Temporal->Estado == BCP_BLOQUEADO_IRQ) {
      Micro.ModuloIRQ.Clientes--;
      Desbloquear(Temporal->Cedula);
    }
  }
  
  asm {
    ldx TempWordC:0      ; // Cargamos la parte alta de SP
    pshx
    pulh
    ldx TempWordC:1      ; // Guardamos la parte alta de SP
    txs                  ; // Regresamos a la pila que estabamos usando
  }

  /* 
    Hay que hacer esto porque el TBM esta configurado a una 
    velocidad demasiado rapida y no deja que se ejecute el 
    proceso inmediatamente posterior
  */
  if(TBM_PENDIENTE)
    TBM_ACUSAR;
}