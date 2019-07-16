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
* ARCHIVO: procesos.c                                                        *
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
* 				                     RAM DE LOS PROCESOS                           *
******************************************************************************
*/

char PilaP0[6] @ (PROC0__RAM-5) = {
  0x12, 0x68, 0xAC, 0x34, (PROC0__ROM>>8)&(0x00FF), (PROC0__ROM&0x00FF)
};

char PilaP1[6] @ (PROC1__RAM-5) = {
  0x12, 0x60, 0xAC, 0x34, (PROC1__ROM>>8)&(0x00FF), (PROC1__ROM&0x00FF)
};

char PilaP2[6] @ (PROC2__RAM-5) = {
  0x12, 0x60, 0xAC, 0x34, (PROC2__ROM>>8)&(0x00FF), (PROC2__ROM&0x00FF)
};
                  
char PilaP3[6] @ (PROC3__RAM-5) = {
  0x12, 0x60, 0xAC, 0x34, (PROC3__ROM>>8)&(0x00FF), (PROC3__ROM&0x00FF)
};

char PilaP4[6] @ (PROC4__RAM-5) = {
  0x12, 0x60, 0xAC, 0x34, (PROC4__ROM>>8)&(0x00FF), (PROC4__ROM&0x00FF)
};

char PilaP5[6] @ (PROC5__RAM-5) = {
  0x12, 0x60, 0xAC, 0x34, (PROC5__ROM>>8)&(0x00FF), (PROC5__ROM&0x00FF)
};

/*
******************************************************************************
*                     VARIABLES GLOBALES DE LOS PROCESOS                     *
******************************************************************************
*/

/* Definiciones para PROCESO VerificarFrenos */
#pragma DATA_SEG DATOS___P0

char TemporalP0;

#pragma DATA_SEG DEFAULT

/* Definiciones para PROCESO MedirBateria */
#pragma DATA_SEG DATOS___P1

#define DOCE_VOLTIOS            0xE6
#define ONCE_VOLTIOS            0xD0
#define DIEZ_VOLTIOS            0xBE
#define OCHO_VOLTIOS            0x98
#define SEIS_VOLTIOS            0x80
#define CERO_VOLTIOS            0x00

char BateriaVoltaje = 0;

#pragma DATA_SEG DEFAULT

/* Definiciones para PROCESO MedirGasolina */
#pragma DATA_SEG DATOS___P2

char GasolinaNivel = 0;

#pragma DATA_SEG DEFAULT

/* Definiciones para PROCESO MedirVelocidad */
#pragma DATA_SEG DATOS___P3

unsigned char TemporalP3 = 0;
unsigned char TemporalDesbordeVel = 0;
unsigned char TemporalDesbordeTIM2 = 0;
unsigned long int TemporalVelocidad = 0;

#pragma DATA_SEG DEFAULT

/* Definiciones para PROCESO MedirRPM */
#pragma DATA_SEG DATOS___P4

unsigned char TemporalP4 = 0;
unsigned char TemporalDesbordeRPM = 0;
unsigned char TemporalDesbordeTIM1 = 0;
unsigned long int TemporalRPM= 0;

#pragma DATA_SEG DEFAULT

/* Definiciones para PROCESO Enviar */
#pragma DATA_SEG DATOS___P5

// Variables del Segmento de Codigo para Codifcar Datos, Creacion de Nibbles
#define BYTES_ENVIAR 4

int TemporalP5 = 0;
unsigned char NIBBLES[2*BYTES_ENVIAR];
                   
//Variable del Segmento de Codigo para la codificacion Hexadecimal
char contador;
unsigned char GRUPOS_HEX[2*BYTES_ENVIAR];      

//Variable del Segmento de Codigo de Variables a Enviar.
unsigned char CODIFICADAS[BYTES_ENVIAR + (BYTES_ENVIAR/2) + 6] = {
  (BYTES_ENVIAR + (BYTES_ENVIAR/2) + 6),0x55,0xFF,0X00,0XFF,
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0xFE
};
//El orden de Mayor a Menor es: RPM_1,RPM_2,VELOCIDAD,ESTADO; Ya estan codificadas y listas para enviar                                 

#pragma DATA_SEG DEFAULT

/* Este es el cosito que se quiere */
CARRO BajaSAEUSB = {
  0, 0, 0  /* Velocidad = 0, RPM = 0, Bateria = 0, Gasolina = 0, Frenos no aplicados */
};

/*
******************************************************************************
* 				                   CODIGO DE LOS PROCESOS                          *
******************************************************************************
*/

/* 
  Los procesos que se ejecutan en el microcontrolador son:

    PROCESOS
    ========
    ----------VerificarFrenos**************
    ----------MedirBateria*****************
    ----------MedirGasolina****************
    ----------ActualizarTablero************
    ----------ModoDeFuncionamiento*********
    ----------MedirVelocidad***************
    ----------MedirRPM*********************
    ----------EnviarPaquete****************
    
    
    DEMONIOS
    ========
    ----------ControlarTablero*************
    ----------OsciladorFrenos**************
*/    

void VerificarFrenos(void) @ "CODIGO__P0" {
  /* 
    CONFIGURACION:
      Nombre de la rutina de interrupcion: isrINT_IRQ
      Interrupciones ocurren: Flanco de bajada y nivel bajo
    VARIABLE:
      CARRO BajaSAEUSB.Estado
        Mirar procesos.h para ver como se verguea esta variable 
  */
   
  for(;;) {
    CARRO_DECLARAR_FRENOS_ACTIVADOS(BajaSAEUSB);
    EnableInterrupts;
    
    while(IRQ_PENDIENTE) {
      IRQ_DESENMASCARAR;
      __RESET_WATCHDOG();
    }    

    DisableInterrupts;
    CARRO_DECLARAR_FRENOS_NO_ACTIVADOS(BajaSAEUSB);
    IRQ_DESENMASCARAR;
    //Nos bloqueamos por IRQ
    __RESET_WATCHDOG();
    TemporalP0 = Sistema((char)IRQ_BLOQUEARSE);
  }
}

void MedirBateria(void) @ "CODIGO__P1" {
  /* 
    CONFIGURACION:
      Nombre de la rutina de interrupcion: isrINT_ADC
      Interrupciones ocurren: Una sola conversión
    VARIABLE:
      CARRO BajaSAEUSB.Estado
        Mirar procesos.h para ver como se verguea esta variable 

          0V  a  6V  --> 0
          6V  a  8V  --> 1
          8V  a  10V --> 2
          10V a  11V --> 3
          mayor  12V --> 4
          
      Se ajusta el trimmer que esta a la entrada del ADC_ADCH4 
      para que con 12v a la salida de la bateria, el ADC tenga
      como salida 0xE6 (230) dejando un margen de seguridad de
      0x19 (25) con respecto a 0dBFS. Se ajustan los demas 
      voltajes (11, 10, 8, 6, 0) con la salida que de el ADC para
      cada uno de ellos.
   */
 
  for(;;) { 
    BateriaVoltaje = Sistema(ADC_LEER_CNL4);

    DisableInterrupts;
    if(BateriaVoltaje >= ONCE_VOLTIOS)
      CARRO_BATERIA_N4(BajaSAEUSB);
    else if (BateriaVoltaje >= DIEZ_VOLTIOS && BateriaVoltaje < ONCE_VOLTIOS)
      CARRO_BATERIA_N3(BajaSAEUSB);
    else if (BateriaVoltaje >= OCHO_VOLTIOS && BateriaVoltaje < DIEZ_VOLTIOS)
      CARRO_BATERIA_N2(BajaSAEUSB);
    else if (BateriaVoltaje >= SEIS_VOLTIOS && BateriaVoltaje < OCHO_VOLTIOS)
      CARRO_BATERIA_N1(BajaSAEUSB);
    else if (BateriaVoltaje >= CERO_VOLTIOS && BateriaVoltaje < SEIS_VOLTIOS)
      CARRO_BATERIA_N0(BajaSAEUSB);
    EnableInterrupts;

    __RESET_WATCHDOG();
  }         
}

void MedirGasolina(void) @ "CODIGO__P2" {
  /* 
    CONFIGURACION:
      Los proximitores del tanque de gasolina estan conectados asi al
      microcontrolador:
  
      Proximitor_4 --> PTD_PTD0 --> Entrada
      Proximitor_3 --> PTD_PTD1 --> Entrada
      Proximitor_2 --> PTD_PTD3 --> Entrada
      Proximitor_1 --> PTD_PTD2 --> Entrada
  
      Los niveles de gasolina se sacan asi:

                PTD3  PTD2  PTD1  PTD0    PTD
      Nivel 4  =  1     1     1     1 --- 0x0F
                  
      
      Nivel 3  =  1     1     1     0 --- 0x0E
                  
                  
      Nivel 2  =  1     1     0     0 --- 0x0C
                  1     1     0     1 --- 0x0D
                  
      Nivel 1  =  0     1     0     0 --- 0x04
                  0     1     0     1 --- 0x05
                  0     1     1     1 --- 0x07
                  
      Nivel 0  =  0     0     0     0 --- 0x00
                  0     0     0     1 --- 0x01
                  0     0     1     1 --- 0x03
                  1     0     1     1 --- 0x0B
    
    VARIABLE:
      CARRO BajaSAEUSB.Estado
        Mirar procesos.h para ver como se verguea esta variable 
   */
  for(;;) {
    GasolinaNivel = PTD;
    GasolinaNivel &= 0x0F;
   
    DisableInterrupts;
    switch(GasolinaNivel) {
      case 0x0F:
        CARRO_GASOLINA_N4(BajaSAEUSB);
        break;
      case 0x0E:
        CARRO_GASOLINA_N3(BajaSAEUSB);
        break;
      case 0x0C:
      case 0x0D:
        CARRO_GASOLINA_N2(BajaSAEUSB);
        break;
      case 0x04:
      case 0x05:
      case 0x07:
        CARRO_GASOLINA_N1(BajaSAEUSB);
        break;
      case 0x00: 
      case 0x01:
      case 0x03:
      case 0x0B:
        CARRO_GASOLINA_N0(BajaSAEUSB);
        break;
      default:
        break;
    }
    EnableInterrupts;
    __RESET_WATCHDOG();
  }           
}

void MedirVelocidad(void) @ "CODIGO__P3" {
  for(;;) {
    /* 
      Primero vemos el estado del TIM2. Al leer el estado, se borra la bandera de 
      TIEMPO_ACTUALIZADO
    */
    TemporalP3 = Sistema(TIM2_CNL0_LEER_ESTADO);
    
    // Verificamos si ya ocurrieron los dos pulsos necesarios para poder medir la velocidad
    if(TemporalP3&BIT_1) {
      TemporalVelocidad = (unsigned long int)Sistema(TIM2_CNL0_LEER_CONTADOR_H)&(0x00FF);
      TemporalVelocidad = TemporalVelocidad<<8;
      TemporalVelocidad |= (unsigned long int)Sistema(TIM2_CNL0_LEER_CONTADOR_L)&(0x00FF);
          
      TemporalVelocidad += (unsigned long int)(TemporalDesbordeVel*65535);
    
      DisableInterrupts;
      // KVEL esta definida en procesos.h
      BajaSAEUSB.Velocidad = (unsigned char)(KVEL/TemporalVelocidad);
      EnableInterrupts;
      
      TemporalDesbordeTIM2 = TemporalDesbordeVel = 0;
    }
    // No han ocurrido los dos pulsos, ya sea que no ha ocurrido el primero o el segundo
    else {
      // Si ocurrio el primero y estamos esperando el segundo pulso, aumentamos Vel
      if(TemporalP3&BIT_0)
        TemporalDesbordeVel++;
      else
        TemporalDesbordeTIM2++;
     
      if((TemporalDesbordeTIM2 == 3) || (TemporalDesbordeVel >= 3)) {
        TemporalDesbordeTIM2 = TemporalDesbordeVel = 0;
      
        DisableInterrupts;
        BajaSAEUSB.Velocidad = 0;
        EnableInterrupts;
        
        TemporalP3 = Sistema(TIM2_CNL0_ESPERAR_PRIMER_PULSO);
      }
    }
    __RESET_WATCHDOG();
    TemporalP3 = Sistema(TIM2_BLOQUEARSE);
  }
}

void MedirRPM(void) @ "CODIGO__P4" {
  for(;;) {
    /* 
      Primero vemos el estado del TIM1. Al leer el estado, se borra la bandera de 
      TIEMPO_ACTUALIZADO
    */
    TemporalP4 = Sistema(TIM1_CNL1_LEER_ESTADO);
    
    // Verificamos si ya ocurrieron los dos pulsos necesarios para poder medir la velocidad
    if(TemporalP4&BIT_1) {
      TemporalRPM = (unsigned long int)Sistema(TIM1_CNL1_LEER_CONTADOR_H)&(0x00FF);
      TemporalRPM = TemporalRPM<<8;
      TemporalRPM |= (unsigned long int)Sistema(TIM1_CNL1_LEER_CONTADOR_L)&(0x00FF);
          
      TemporalRPM += (unsigned long int)(TemporalDesbordeRPM*65535);
    
      DisableInterrupts;
      // KRPM esta definida en procesos.h
      BajaSAEUSB.RPM = (unsigned int)(KRPM/TemporalRPM);
      EnableInterrupts;
      
      TemporalDesbordeTIM1 = TemporalDesbordeRPM = 0;
    }
    // No han ocurrido los dos pulsos, ya sea que no ha ocurrido el primero o el segundo
    else {
      if(TemporalP4&BIT_0)
        TemporalDesbordeRPM++;
      else
        TemporalDesbordeTIM1++;
      
      if((TemporalDesbordeTIM1 == 1) || (TemporalDesbordeRPM == 1)) {
        TemporalDesbordeTIM1 = TemporalDesbordeRPM = 0;
      
        DisableInterrupts;
        BajaSAEUSB.RPM = 0;
        EnableInterrupts;

        TemporalP4 = Sistema(TIM1_CNL1_ESPERAR_PRIMER_PULSO);
      }
    }
    __RESET_WATCHDOG();
    TemporalP4 = Sistema(TIM1_BLOQUEARSE);
  }
}

void EnviarPaquete(void) @ "CODIGO__P5" {
  TemporalP5 = (unsigned int)&CODIFICADAS;
  asm {
    ldhx TemporalP5
    lda #SCI_ESTABLECER_BUFFER
    jsr Sistema
  }

  CODIFICADAS[0] = 0x55;
  SCI_HABLTR_INT_SCDR_VACIO;     
  
  for(;;) {
    /* Aqui deberia ir el codigo de este vergo */
    CrearNibbles();   
    CrearHex();
    CrearCodificado();
    __RESET_WATCHDOG();
  }
}
    
void CrearNibbles(void) @ "CODIGO__P5" {
  DisableInterrupts;
  NIBBLES[7]=(char)((BajaSAEUSB.Estado)&(0x0F));
  NIBBLES[6]=(char)(((BajaSAEUSB.Estado)&(0xF0))>>4);
  NIBBLES[5]=(char)((BajaSAEUSB.Velocidad)&(0x0F));
  NIBBLES[4]=(char)(((BajaSAEUSB.Velocidad)&(0xF0))>>4);
  NIBBLES[3]=(char)((BajaSAEUSB.RPM)&(0x000F));
  NIBBLES[2]=(char)(((BajaSAEUSB.RPM)&(0x00F0))>>4);
  NIBBLES[1]=(char)(((BajaSAEUSB.RPM)&(0x0F00))>>8);
  NIBBLES[0]=(char)(((BajaSAEUSB.RPM)&(0xF000))>>12);
  EnableInterrupts;
}

void CrearHex(void) @ "CODIGO__P5" {
  for(contador=0; contador < (2*BYTES_ENVIAR); contador++) {
    if(NIBBLES[contador] <= 8) {
      if(NIBBLES[contador] <= 4) {
        if(NIBBLES[contador] <= 2) {
          if(NIBBLES[contador] <= 1) {
            if(NIBBLES[contador] == 1) {
              GRUPOS_HEX[contador] = 0x0B;
              //convierto el equivalente de 1
            } 
            else if(NIBBLES[contador] == 0)
              GRUPOS_HEX[contador] = 0x07;
              //convierto el equivalente de 0
          }
          else if(NIBBLES[contador] == 2)
            GRUPOS_HEX[contador] = 0x0D;
            //convierto el equivalente de 2
        }
        if(NIBBLES[contador] == 3) {
          GRUPOS_HEX[contador] = 0x0E;
          //convierto el equivalente de 3
        } 
        else if(NIBBLES[contador] == 4)
          GRUPOS_HEX[contador] = 0x13;
          //convierto el equivalente 4
      }
      if(NIBBLES[contador] <= 6) {
        if(NIBBLES[contador] == 5) {
          GRUPOS_HEX[contador] = 0x15;
          //convierto el equivalente de 5
        }
        else if(NIBBLES[contador] == 6)
          GRUPOS_HEX[contador] = 0x16;
          //convierto el equivalente de 6
      }
      if(NIBBLES[contador] == 7) { 
        GRUPOS_HEX[contador] = 0x19;
        //convierto el equivalente de 7
      } 
      else if(NIBBLES[contador]==8)
        GRUPOS_HEX[contador] = 0x1A;
        //convertir el equivalente de 8
    }
    if(NIBBLES[contador] <= 12) {
      if(NIBBLES[contador] <= 10) {
        if(NIBBLES[contador] == 9) {
          GRUPOS_HEX[contador] = 0x1C;
          //convertir igual a 9
        } 
        else if(NIBBLES[contador]==10)
          GRUPOS_HEX[contador] = 0x23;
          //convertir igual a 10
      }
      if(NIBBLES[contador] == 11) {
        GRUPOS_HEX[contador] = 0x25;
        //convertir igual a 11
      } 
      else if(NIBBLES[contador]==12)
        GRUPOS_HEX[contador] = 0x26; 
        //convertir igual a 12
    }
    if(NIBBLES[contador] <= 14) {
      if(NIBBLES[contador] == 13) {
        GRUPOS_HEX[contador] = 0x29;
        //convertir igual a 13
      } 
      else if(NIBBLES[contador] == 14)
        GRUPOS_HEX[contador] = 0x2A;
        //convertir igual a 14  
    }
    else if(NIBBLES[contador] == 15)
      GRUPOS_HEX[contador] = 0x2C;
      //convertir igual a 15
  }
}

void CrearCodificado(void) @ "CODIGO__P5" {
   DisableInterrupts;
   CODIFICADAS[5] = (((GRUPOS_HEX[0]<<2)&0xFC)|((GRUPOS_HEX[1]>>4)&0x03));
   CODIFICADAS[6] = (((GRUPOS_HEX[1]<<4)&0xF0)|((GRUPOS_HEX[2]>>2)&0x0F));
   CODIFICADAS[7] = (((GRUPOS_HEX[2]<<6)&0xC0)|((GRUPOS_HEX[3]&0x3F)));
   CODIFICADAS[8] = (((GRUPOS_HEX[4]<<2)&0xFC)|((GRUPOS_HEX[5]>>4)&0x03));
   CODIFICADAS[9] = (((GRUPOS_HEX[5]<<4)&0xF0)|((GRUPOS_HEX[6]>>2)&0x0F));
   CODIFICADAS[10] = (((GRUPOS_HEX[6]<<6)&0xC0)|((GRUPOS_HEX[7]&0x3F)));
   EnableInterrupts;
}