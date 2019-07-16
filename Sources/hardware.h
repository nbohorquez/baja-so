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
* ARCHIVO: hardware.h                                                        *
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

#ifndef _HARDWARE_H_
#define _HARDWARE_H_

/*
******************************************************************************
* 				                  DEFINICIONES DE HARDWARE                         *
******************************************************************************
*/

/* 
  Cuando ocurre una interrupcion, el procesador guarda todos los registros
  del proceso en la pila de la siguiente forma:
  
  *-------------*
  |     ...     | <---- SP
  *-------------* 
  |      H      | <---- SP[1] === 1,SP
  *-------------*
  |     CCR     | <---- SP[2] === 2,SP
  *-------------*
  |     ACC     | <---- SP[3] === 3,SP
  *-------------*
  |      X      | <---- SP[4] === 4,SP
  *-------------*
  |    PC_H     | <---- SP[5] === 5,SP
  *-------------*
  |    PC_L     | <---- SP[6] === 6,SP
  *-------------*
  |     ...     |
  *-------------*
*/

#define PILA_H    1
#define PILA_CCR  2
#define PILA_ACC  3
#define PILA_X    4
#define PILA_PCH  5
#define PILA_PCL  6

/* 
   CONVERSOR ANALOGICO/DIGITAL (ADC) --> nomenclatura ADC_NNNNNN
*/

#define ADSCR_BIT_ADCH0       ((1<<0)&0xFF)
#define ADSCR_BIT_ADCH1       ((1<<1)&0xFF)
#define ADSCR_BIT_ADCH2       ((1<<2)&0xFF)
#define ADSCR_BIT_ADCH3       ((1<<3)&0xFF)
#define ADSCR_BIT_ADCH4       ((1<<4)&0xFF)
#define ADSCR_BIT_ADCO        ((1<<5)&0xFF) /* Conversión continua = 1. Una sola conversión = 0 */
#define ADSCR_BIT_AIEN        ((1<<6)&0xFF) /* Int ADC habilitada = 1. Int ADC deshabilitada = 0 */
#define ADSCR_BIT_COCO        ((1<<7)&0xFF) /* Conversion completa(AIEN=0) = 1. Conversion no completa(AIEN=0)/Int CPU(AIEN=1) = 0 */

#define ADCLK_BIT_ADICLK      ((1<<4)&0xFF) /* Reloj del bus interno = 1. Reloj externo (CGMXCLK) = 0 */
#define ADCLK_BIT_ADIV0       ((1<<5)&0xFF) /* Divisor utilizado por el ADC para generar el reloj del ADC */ 
#define ADCLK_BIT_ADIV1       ((1<<6)&0xFF)
#define ADCLK_BIT_ADIV2       ((1<<7)&0xFF)

#define ADSCR_MASCARA_ADCH    (ADSCR_BIT_ADCH0|ADSCR_BIT_ADCH1|ADSCR_BIT_ADCH2|ADSCR_BIT_ADCH3|ADSCR_BIT_ADCH4)

#define ADC_PTB0_AD0          (ADSCR &= ~ADSCR_MASCARA_ADCH)                    /* 0 0 0 0 0 */
#define ADC_PTB1_AD1          (ADSCR = ((ADSCR&(~ADSCR_MASCARA_ADCH))|0x01))    /* 0 0 0 0 1 */
#define ADC_PTB2_AD2          (ADSCR = ((ADSCR&(~ADSCR_MASCARA_ADCH))|0x02))    /* 0 0 0 1 0 */
#define ADC_PTB3_AD3          (ADSCR = ((ADSCR&(~ADSCR_MASCARA_ADCH))|0x03))    /* 0 0 0 1 1 */
#define ADC_PTB4_AD4          (ADSCR = ((ADSCR&(~ADSCR_MASCARA_ADCH))|0x04))    /* 0 0 1 0 0 */
#define ADC_PTB5_AD5          (ADSCR = ((ADSCR&(~ADSCR_MASCARA_ADCH))|0x05))    /* 0 0 1 0 1 */
#define ADC_PTB6_AD6          (ADSCR = ((ADSCR&(~ADSCR_MASCARA_ADCH))|0x06))    /* 0 0 1 1 0 */
#define ADC_PTB7_AD7          (ADSCR = ((ADSCR&(~ADSCR_MASCARA_ADCH))|0x07))    /* 0 0 1 1 1 */
#define ADC_VREFH             (ADSCR = ((ADSCR&(~ADSCR_MASCARA_ADCH))|0x1D))    /* 1 1 1 0 1 */
#define ADC_VREFL             (ADSCR = ((ADSCR&(~ADSCR_MASCARA_ADCH))|0x1E))    /* 1 1 1 1 0 */
#define ADC_APAGAR            (ADSCR = ((ADSCR&(~ADSCR_MASCARA_ADCH))|0x1F))    /* 1 1 1 1 1 */

#define ADC_ACTIVAR_INT       (ADSCR |= ADSCR_BIT_AIEN)
#define ADC_DESACTIVAR_INT    (ADSCR &= ~ADSCR_BIT_AIEN)
#define ADC_CONV_CONTINUA     (ADSCR |= ADSCR_BIT_ADCO)
#define ADC_CONV_SIMPLE       (ADSCR &= ~ADSCR_BIT_ADCO)
#define ADC_ACUSAR            (ADSCR &= ~ADSCR_BIT_COCO)

typedef struct s_adc {
  char Clientes;
  char Estado;
} ADC;

/* 
  Configuracion de bits de ADC.Estado
  
     b7     b6     b5     b4     b3     b2     b1     b0   
  | XXXX | XXXX | XXXX | XXXX | XXXX | XXXX | DATO | OCUP |
  |                  LIBRES                 |             |
   

DATO = 1 --> Dato Entregado
DATO = 0 --> Dato No Entregado

OCUP = 1 --> ADC Ocupado
OCUP = 0 --> ADC Libre
*/

#define ADC_ESTADO_BIT_OCUP               ((1<<0)&0xFF)
#define ADC_ESTADO_BIT_DATO               ((1<<1)&0xFF)

// Estos son para hacer preguntas tipo "if"
#define ADC_DATO_ENTREGADO(i)             (i.Estado&ADC_ESTADO_BIT_DATO)
#define ADC_OCUPADO(i)                    (i.Estado&ADC_ESTADO_BIT_OCUP)

// Estos son para declarar estados
#define ADC_OCUPAR(i)                     (i.Estado |= ADC_ESTADO_BIT_OCUP)
#define ADC_LIBERAR(i)                    (i.Estado &= ~ADC_ESTADO_BIT_OCUP)
#define ADC_DECLARAR_DATO_ENTREGADO(i)    (i.Estado |= ADC_ESTADO_BIT_DATO) 
#define ADC_DECLARAR_DATO_NO_ENTREGADO(i) (i.Estado &= ~ADC_ESTADO_BIT_DATO)

/* 
   MODULO DE COMUNICACION SERIAL (SCI) --> nomenclatura SCI_NNNNNN
*/
#define SCC1_BIT_LOOPS        ((1<<7)&0xFF)
#define SCC1_BIT_ENSCI        ((1<<6)&0xFF)
#define SCC1_BIT_TXINV        ((1<<5)&0xFF)
#define SCC1_BIT_M            ((1<<4)&0xFF)
#define SCC1_BIT_WAKE         ((1<<3)&0xFF)
#define SCC1_BIT_ILTY         ((1<<2)&0xFF)
#define SCC1_BIT_PEN          ((1<<1)&0xFF)
#define SCC1_BIT_PTY          ((1<<0)&0xFF)



#define SCC2_BIT_SCTIE        ((1<<7)&0xFF)
#define SCC2_BIT_TCIE         ((1<<6)&0xFF)
#define SCC2_BIT_SCRIE        ((1<<5)&0xFF)
#define SCC2_BIT_ILIE         ((1<<4)&0xFF)
#define SCC2_BIT_TE           ((1<<3)&0xFF)
#define SCC2_BIT_RE           ((1<<2)&0xFF)
#define SCC2_BIT_RWU          ((1<<1)&0xFF)
#define SCC2_BIT_SBK          ((1<<0)&0xFF)

#define SCI_HABLTR_INT_SCDR_VACIO   (SCC2 |= SCC2_BIT_SCTIE)
#define SCI_DESHAB_INT_SCDR_VACIO   (SCC2 &= ~SCC2_BIT_SCTIE)
#define SCI_HABLTR_TX               (SCC2 |= SCC2_BIT_TE)
#define SCI_DESHAB_TX               (SCC2 &= ~SCC2_BIT_TE)

#define SCC3_BIT_R8           ((1<<7)&0xFF)
#define SCC3_BIT_T8           ((1<<6)&0xFF)
#define SCC3_BIT_DMARE        ((1<<5)&0xFF)
#define SCC3_BIT_DMATE        ((1<<4)&0xFF)
#define SCC3_BIT_ORIE         ((1<<3)&0xFF)
#define SCC3_BIT_NEIE         ((1<<2)&0xFF)
#define SCC3_BIT_FEIE         ((1<<1)&0xFF)
#define SCC3_BIT_PEIE         ((1<<0)&0xFF)



#define SCS1_BIT_SCTE         ((1<<7)&0xFF)
#define SCS1_BIT_TC           ((1<<6)&0xFF)
#define SCS1_BIT_SCRF         ((1<<5)&0xFF)
#define SCS1_BIT_IDLE         ((1<<4)&0xFF)
#define SCS1_BIT_OR           ((1<<3)&0xFF)
#define SCS1_BIT_NF           ((1<<2)&0xFF)
#define SCS1_BIT_FE           ((1<<1)&0xFF)
#define SCS1_BIT_PE           ((1<<0)&0xFF)



#define SCS2_BIT_BKF          ((1<<1)&0xFF)
#define SCS2_BIT_RPF          ((1<<0)&0xFF)



#define SCBR_BIT_SCP1         ((1<<5)&0xFF)
#define SCBR_BIT_SCP0         ((1<<4)&0xFF)
#define SCBR_BIT_R            ((1<<3)&0xFF)
#define SCBR_BIT_SCR2         ((1<<2)&0xFF)
#define SCBR_BIT_SCR1         ((1<<1)&0xFF)
#define SCBR_BIT_SCR0         ((1<<0)&0xFF)

typedef struct s_sci {
  char *Buffer;
  char TamanoBuffer;
  char Indice;
} SCI;

/* 
   MODULO DE INTERRUPCIONES EXTERNAS (IRQ) --> nomenclatura IRQ_NNNNNN
*/

#define INTSCR_BIT_IRQF       ((1<<3)&0xFF)
#define INTSCR_BIT_ACK        ((1<<2)&0xFF)
#define INTSCR_BIT_IMASK      ((1<<1)&0xFF)
#define INTSCR_BIT_MODE       ((1<<0)&0xFF)

#define IRQ_PENDIENTE         (INTSCR&INTSCR_BIT_IRQF)
#define IRQ_DESENMASCARAR     (INTSCR &= ~INTSCR_BIT_IMASK)
#define IRQ_ENMASCARAR        (INTSCR |= INTSCR_BIT_IMASK)
#define IRQ_ACUSAR            (INTSCR |= INTSCR_BIT_ACK)
#define IRQ_MODO_NIVEL        (INTSCR |= INTSCR_BIT_MODE)
#define IRQ_MODO_FLANCO       (INTSCR &= ~INTSCR_BIT_MODE)

typedef struct s_irq {
  char Clientes;
  char Estado;
} IRQ;

/* 
   MODULO DE BASE DE TIEMPO (TBM) --> nomenclatura TBM_NNNNNN
*/

#define TBCR_BIT_TBIF         ((1<<7)&0xFF)
#define TBCR_BIT_TACK         ((1<<3)&0xFF)
#define TBCR_BIT_TBIE         ((1<<2)&0xFF)
#define TBCR_BIT_TBON         ((1<<1)&0xFF)

#define TBM_PENDIENTE         (TBCR&TBCR_BIT_TBIF)
#define TBM_ACUSAR            (TBCR |= TBCR_BIT_TACK)
#define TBM_HABILITAR         (TBCR |= TBCR_BIT_TBIE)
#define TBM_DESHABILITAR      (TBCR &= ~TBCR_BIT_TBIE)
#define TBM_ENCENDER          (TBCR |= TBCR_BIT_TBON)
#define TBM_APAGAR            (TBCR &= ~TBCR_BIT_TBON)

/* 
   REGISTROS DEL TIM
*/

#define TSC_BIT_TOF           ((1<<7)&0xFF)
#define TSC_BIT_TOIE          ((1<<6)&0xFF)
#define TSC_BIT_TSTOP         ((1<<5)&0xFF)
#define TSC_BIT_TRST          ((1<<4)&0xFF)
#define TSC_BIT_PS2           ((1<<2)&0xFF)
#define TSC_BIT_PS1           ((1<<1)&0xFF)
#define TSC_BIT_PS0           ((1<<0)&0xFF)

#define TSCx_BIT_CHxF         ((1<<7)&0xFF)
#define TSCx_BIT_CHxIE        ((1<<6)&0xFF)
#define TSCx_BIT_MSxB         ((1<<5)&0xFF)
#define TSCx_BIT_MSxA         ((1<<4)&0xFF)
#define TSCx_BIT_ELSxB        ((1<<3)&0xFF)
#define TSCx_BIT_ELSxA        ((1<<2)&0xFF)
#define TSCx_BIT_TOVx         ((1<<1)&0xFF)
#define TSCx_BIT_CHxMAX       ((1<<0)&0xFF)

typedef struct s_canal {
  char Estado;
  unsigned char Contador_H;
  unsigned char Contador_L;
} CANAL;

typedef struct s_tim {
  char Clientes;
  CANAL CNL0;
  CANAL CNL1;
} TIM;

/* Configuración de TIMx.CNLx_Estado

     b7     b6     b5     b4     b3     b2     b1     b0   
  | XXXX | XXXX | XXXX | XXXX | XXXX | XXXX | TIEM | PULS |
  |              LIBRES                     |             |

TIEM = 0 --> El TIM no ha registrado dos pulsos, por lo que no puede calcular el intervalo
       1 --> El TIM ha registrado dos pulsos, por lo que puede calcular el intervalo

PULS = 0 --> Se Esta Esperando El Primer Pulso Para El Conteo
       1 --> Se Esta Esperando El Segundo Pulso Para El Conteo
*/

#define TIM_ESTADO_BIT_TIEM             ((1<<1)&0xFF)
#define TIM_ESTADO_BIT_PULS             ((1<<0)&0xFF)

// Estos son para hacer preguntas tipo "if"
#define TIM_ESPERANDO_PRIMER_PULSO(i)   (!(i.Estado&TIM_ESTADO_BIT_PULS))
#define TIM_ESPERANDO_SEGUNDO_PULSO(i)  (i.Estado&TIM_ESTADO_BIT_PULS)
#define TIM_TIEMPO_ACTUALIZADO(i)       (i.Estado&TIM_ESTADO_BIT_TIEM)
#define TIM_TIEMPO_NO_ACTUALIZADO(i)    (!(i.Estado&TIM_ESTADO_BIT_TIEM))

// Estos son para declarar estados
#define TIM_ESPERAR_PRIMER_PULSO(i)             (i.Estado &= ~TIM_ESTADO_BIT_PULS)
#define TIM_ESPERAR_SEGUNDO_PULSO(i)            (i.Estado |= TIM_ESTADO_BIT_PULS)
#define TIM_DECLARAR_TIEMPO_ACTUALIZADO(i)      (i.Estado |= TIM_ESTADO_BIT_TIEM)
#define TIM_DECLARAR_TIEMPO_NO_ACTUALIZADO(i)   (i.Estado &= ~TIM_ESTADO_BIT_TIEM)

/* 
   MODULO DE INTERFAZ CON EL TEMPORIZADOR 1 (TIM1) --> nomenclatura TIM1_NNNNNN
*/

#define TIM1_BORRAR_DESBORDE  (T1SC &= ~TSC_BIT_TOF)
#define TIM1_HABLTR_DESBORDE  (T1SC |= TSC_BIT_TOIE)
#define TIM1_DESHAB_DESBORDE  (T1SC &= ~TSC_BIT_TOIE)
#define TIM1_DETENER          (T1SC |= TSC_BIT_TSTOP)
#define TIM1_INICIAR          (T1SC &= ~TSC_BIT_TSTOP)
#define TIM1_VOLVER_A_CERO    (T1SC |= TSC_BIT_TRST)

/* Definiciones para TIM1 - CANAL 0 */

#define TIM1_CNL0_NO_ENT_CAPTR                (T1SC0 &= ~TSCx_BIT_CHxF)
#define TIM1_CNL0_NO_SAL_COMP                 (T1SC0 &= ~TSCx_BIT_CHxF)
#define TIM1_CNL0_HABLTR_INT                  (T1SC0 |= TSCx_BIT_CHxIE)
#define TIM1_CNL0_DESHAB_INT                  (T1SC0 &= ~TSCx_BIT_CHxIE)
#define TIM1_CNL0_HABLTR_PWM_BUF              (T1SC0 |= TSCx_BIT_MSxB)
#define TIM1_CNL0_DESHAB_PWM_BUF              (T1SC0 &= ~TSCx_BIT_MSxB)
#define TIM1_CNL0_HABLTR_SAL_COMP_BUF         (T1SC0 |= TSCx_BIT_MSxB)
#define TIM1_CNL0_DESHAB_SAL_COMP_BUF         (T1SC0 &= ~TSCx_BIT_MSxB)
#define TIM1_CNL0_HABLTR_SAL_COMP_NOBUF       (T1SC0 |= TSCx_BIT_MSxA)  
#define TIM1_CNL0_HABLTR_PWM_NOBUF            (T1SC0 |= TSCx_BIT_MSxA)
#define TIM1_CNL0_HABLTR_ENT_CAPTR            (T1SC0 &= ~TSCx_BIT_MSxA)
#define TIM1_CNL0_VALOR_INICIAL_BAJO          (T1SC0 |= TSCx_BIT_MSxA)
#define TIM1_CNL0_VALOR_INICIAL_ALTO          (T1SC0 &= ~TSCx_BIT_MSxA)
#define TIM1_CNL0_CAMBIAR_EN_DESBORDE         (T1SC0 |= TSCx_BIT_TOVx)
#define TIM1_CNL0_NOCAMBIAR_EN_DESBORDE       (T1SC0 &= ~TSCx_BIT_TOVx)
#define TIM1_CNL0_CICLO_TRABAJO_100           (T1SC0 |= TSCx_BIT_CHxMAX)
#define TIM1_CNL0_CICLO_TRABAJO_0             (T1SC0 &= ~TSCx_BIT_CHxMAX)

#define TIM1_CNL0_MASC_MODO                   (TSCx_BIT_MSxB|TSCx_BIT_MSxA|TSCx_BIT_ELSxB|TSCx_BIT_ELSxA)

#define TIM1_CNL0_MODO_PTO_ALTO               (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b0000<<2))
#define TIM1_CNL0_MODO_PTO_BAJO               (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b0100<<2))
#define TIM1_CNL0_MODO_ENT_FLANCO_ALTO        (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b0001<<2))
#define TIM1_CNL0_MODO_ENT_FLANCO_BAJO        (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b0010<<2))
#define TIM1_CNL0_MODO_ENT_FLANCO_AMBOS       (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b0011<<2))
#define TIM1_CNL0_MODO_SALNOBUF_CAMBIAR_COMP  (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b0101<<2))
#define TIM1_CNL0_MODO_SALNOBUF_CERO_COMP     (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b0110<<2))
#define TIM1_CNL0_MODO_SALNOBUF_UNO_COMP      (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b0111<<2))
#define TIM1_CNL0_MODO_PWMNOBUF_CAMBIAR_COMP  (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b0101<<2))
#define TIM1_CNL0_MODO_PWMNOBUF_CERO_COMP     (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b0110<<2))
#define TIM1_CNL0_MODO_PWMNOBUF_UNO_COMP      (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b0111<<2))
#define TIM1_CNL0_MODO_SALBUF_CAMBIAR_COMP    (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b1001<<2))
#define TIM1_CNL0_MODO_SALBUF_CERO_COMP       (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b1010<<2))
#define TIM1_CNL0_MODO_SALBUF_UNO_COMP        (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b1011<<2))
#define TIM1_CNL0_MODO_PWMBUF_CAMBIAR_COMP    (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b1001<<2))
#define TIM1_CNL0_MODO_PWMBUF_CERO_COMP       (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b1010<<2))
#define TIM1_CNL0_MODO_PWMBUF_UNO_COMP        (T1SC0 = ((T1SC0)&(~TIM1_CNL0_MASC_MODO))|(0b1011<<2))

/* Definiciones para TIM1 - CANAL 1 */

#define TIM1_CNL1_NO_ENT_CAPTR                (T1SC1 &= ~TSCx_BIT_CHxF)
#define TIM1_CNL1_NO_SAL_COMP                 (T1SC1 &= ~TSCx_BIT_CHxF)
#define TIM1_CNL1_HABLTR_INT                  (T1SC1 |= TSCx_BIT_CHxIE)
#define TIM1_CNL1_DESHAB_INT                  (T1SC1 &= ~TSCx_BIT_CHxIE)
#define TIM1_CNL1_HABLTR_SAL_COMP_NOBUF       (T1SC1 |= TSCx_BIT_MSxA)  
#define TIM1_CNL1_HABLTR_PWM_NOBUF            (T1SC1 |= TSCx_BIT_MSxA)
#define TIM1_CNL1_HABLTR_ENT_CAPTR            (T1SC1 &= ~TSCx_BIT_MSxA)
#define TIM1_CNL1_VALOR_INICIAL_BAJO          (T1SC1 |= TSCx_BIT_MSxA)
#define TIM1_CNL1_VALOR_INICIAL_ALTO          (T1SC1 &= ~TSCx_BIT_MSxA)
#define TIM1_CNL1_CAMBIAR_EN_DESBORDE         (T1SC1 |= TSCx_BIT_TOVx)
#define TIM1_CNL1_NOCAMBIAR_EN_DESBORDE       (T1SC1 &= ~TSCx_BIT_TOVx)
#define TIM1_CNL1_CICLO_TRABAJO_100           (T1SC1 |= TSCx_BIT_CHxMAX)
#define TIM1_CNL1_CICLO_TRABAJO_0             (T1SC1 &= ~TSCx_BIT_CHxMAX)

#define TIM1_CNL1_MASC_MODO                   (TSCx_BIT_MSxA|TSCx_BIT_ELSxB|TSCx_BIT_ELSxA)

#define TIM1_CNL1_MODO_PTO_ALTO               (T1SC1 = ((T1SC1)&(~TIM1_CNL1_MASC_MODO))|(0b000<<2))
#define TIM1_CNL1_MODO_PTO_BAJO               (T1SC1 = ((T1SC1)&(~TIM1_CNL1_MASC_MODO))|(0b100<<2))
#define TIM1_CNL1_MODO_ENT_FLANCO_ALTO        (T1SC1 = ((T1SC1)&(~TIM1_CNL1_MASC_MODO))|(0b001<<2))
#define TIM1_CNL1_MODO_ENT_FLANCO_BAJO        (T1SC1 = ((T1SC1)&(~TIM1_CNL1_MASC_MODO))|(0b010<<2))
#define TIM1_CNL1_MODO_ENT_FLANCO_AMBOS       (T1SC1 = ((T1SC1)&(~TIM1_CNL1_MASC_MODO))|(0b011<<2))
#define TIM1_CNL1_MODO_SALNOBUF_CAMBIAR_COMP  (T1SC1 = ((T1SC1)&(~TIM1_CNL1_MASC_MODO))|(0b101<<2))
#define TIM1_CNL1_MODO_SALNOBUF_CERO_COMP     (T1SC1 = ((T1SC1)&(~TIM1_CNL1_MASC_MODO))|(0b110<<2))
#define TIM1_CNL1_MODO_SALNOBUF_UNO_COMP      (T1SC1 = ((T1SC1)&(~TIM1_CNL1_MASC_MODO))|(0b111<<2))
#define TIM1_CNL1_MODO_PWMNOBUF_CAMBIAR_COMP  (T1SC1 = ((T1SC1)&(~TIM1_CNL1_MASC_MODO))|(0b101<<2))
#define TIM1_CNL1_MODO_PWMNOBUF_CERO_COMP     (T1SC1 = ((T1SC1)&(~TIM1_CNL1_MASC_MODO))|(0b110<<2))
#define TIM1_CNL1_MODO_PWMNOBUF_UNO_COMP      (T1SC1 = ((T1SC1)&(~TIM1_CNL1_MASC_MODO))|(0b111<<2))

/* 
   MODULO DE INTERFAZ CON EL TEMPORIZADOR 2 (TIM1) --> nomenclatura TIM2_NNNNNN
*/

#define TIM2_BORRAR_DESBORDE  (T2SC &= ~TSC_BIT_TOF)
#define TIM2_HABLTR_DESBORDE  (T2SC |= TSC_BIT_TOIE)
#define TIM2_DESHAB_DESBORDE  (T2SC &= ~TSC_BIT_TOIE)
#define TIM2_DETENER          (T2SC |= TSC_BIT_TSTOP)
#define TIM2_INICIAR          (T2SC &= ~TSC_BIT_TSTOP)
#define TIM2_VOLVER_A_CERO    (T2SC |= TSC_BIT_TRST)

// DEFINICIONES PARA EL TIM2 - CANAL 0

#define TIM2_CNL0_NO_ENT_CAPTR                (T2SC0 &= ~TSCx_BIT_CHxF)
#define TIM2_CNL0_NO_SAL_COMP                 (T2SC0 &= ~TSCx_BIT_CHxF)
#define TIM2_CNL0_HABLTR_INT                  (T2SC0 |= TSCx_BIT_CHxIE)
#define TIM2_CNL0_DESHAB_INT                  (T2SC0 &= ~TSCx_BIT_CHxIE)
#define TIM2_CNL0_HABLTR_PWM_BUF              (T2SC0 |= TSCx_BIT_MSxB)
#define TIM2_CNL0_DESHAB_PWM_BUF              (T2SC0 &= ~TSCx_BIT_MSxB)
#define TIM2_CNL0_HABLTR_SAL_COMP_BUF         (T2SC0 |= TSCx_BIT_MSxB)
#define TIM2_CNL0_DESHAB_SAL_COMP_BUF         (T2SC0 &= ~TSCx_BIT_MSxB)
#define TIM2_CNL0_HABLTR_SAL_COMP_NOBUF       (T2SC0 |= TSCx_BIT_MSxA)  
#define TIM2_CNL0_HABLTR_PWM_NOBUF            (T2SC0 |= TSCx_BIT_MSxA)
#define TIM2_CNL0_HABLTR_ENT_CAPTR            (T2SC0 &= ~TSCx_BIT_MSxA)
#define TIM2_CNL0_VALOR_INICIAL_BAJO          (T2SC0 |= TSCx_BIT_MSxA)
#define TIM2_CNL0_VALOR_INICIAL_ALTO          (T2SC0 &= ~TSCx_BIT_MSxA)
#define TIM2_CNL0_CAMBIAR_EN_DESBORDE         (T2SC0 |= TSCx_BIT_TOVx)
#define TIM2_CNL0_NOCAMBIAR_EN_DESBORDE       (T2SC0 &= ~TSCx_BIT_TOVx)
#define TIM2_CNL0_CICLO_TRABAJO_100           (T2SC0 |= TSCx_BIT_CHxMAX)
#define TIM2_CNL0_CICLO_TRABAJO_0             (T2SC0 &= ~TSCx_BIT_CHxMAX)

#define TIM2_CNL0_MASC_MODO                   (TSCx_BIT_MSxB|TSCx_BIT_MSxA|TSCx_BIT_ELSxB|TSCx_BIT_ELSxA)

#define TIM2_CNL0_MODO_PTO_ALTO               (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b0000<<2))
#define TIM2_CNL0_MODO_PTO_BAJO               (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b0100<<2))
#define TIM2_CNL0_MODO_ENT_FLANCO_ALTO        (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b0001<<2))
#define TIM2_CNL0_MODO_ENT_FLANCO_BAJO        (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b0010<<2))
#define TIM2_CNL0_MODO_ENT_FLANCO_AMBOS       (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b0011<<2))
#define TIM2_CNL0_MODO_SALNOBUF_CAMBIAR_COMP  (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b0101<<2))
#define TIM2_CNL0_MODO_SALNOBUF_CERO_COMP     (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b0110<<2))
#define TIM2_CNL0_MODO_SALNOBUF_UNO_COMP      (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b0111<<2))
#define TIM2_CNL0_MODO_PWMNOBUF_CAMBIAR_COMP  (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b0101<<2))
#define TIM2_CNL0_MODO_PWMNOBUF_CERO_COMP     (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b0110<<2))
#define TIM2_CNL0_MODO_PWMNOBUF_UNO_COMP      (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b0111<<2))
#define TIM2_CNL0_MODO_SALBUF_CAMBIAR_COMP    (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b1001<<2))
#define TIM2_CNL0_MODO_SALBUF_CERO_COMP       (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b1010<<2))
#define TIM2_CNL0_MODO_SALBUF_UNO_COMP        (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b1011<<2))
#define TIM2_CNL0_MODO_PWMBUF_CAMBIAR_COMP    (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b1001<<2))
#define TIM2_CNL0_MODO_PWMBUF_CERO_COMP       (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b1010<<2))
#define TIM2_CNL0_MODO_PWMBUF_UNO_COMP        (T2SC0 = ((T2SC0)&(~TIM2_CNL0_MASC_MODO))|(0b1011<<2))

// DEFINICIONES PARA EL TIM2 - CANAL 1

#define TIM2_CNL1_NO_ENT_CAPTR                (T2SC1 &= ~TSCx_BIT_CHxF)
#define TIM2_CNL1_NO_SAL_COMP                 (T2SC1 &= ~TSCx_BIT_CHxF)
#define TIM2_CNL1_HABLTR_INT                  (T2SC1 |= TSCx_BIT_CHxIE)
#define TIM2_CNL1_DESHAB_INT                  (T2SC1 &= ~TSCx_BIT_CHxIE)
#define TIM2_CNL1_HABLTR_SAL_COMP_NOBUF       (T2SC1 |= TSCx_BIT_MSxA)  
#define TIM2_CNL1_HABLTR_PWM_NOBUF            (T2SC1 |= TSCx_BIT_MSxA)
#define TIM2_CNL1_HABLTR_ENT_CAPTR            (T2SC1 &= ~TSCx_BIT_MSxA)
#define TIM2_CNL1_VALOR_INICIAL_BAJO          (T2SC1 |= TSCx_BIT_MSxA)
#define TIM2_CNL1_VALOR_INICIAL_ALTO          (T2SC1 &= ~TSCx_BIT_MSxA)
#define TIM2_CNL1_CAMBIAR_EN_DESBORDE         (T2SC1 |= TSCx_BIT_TOVx)
#define TIM2_CNL1_NOCAMBIAR_EN_DESBORDE       (T2SC1 &= ~TSCx_BIT_TOVx)
#define TIM2_CNL1_CICLO_TRABAJO_100           (T2SC1 |= TSCx_BIT_CHxMAX)
#define TIM2_CNL1_CICLO_TRABAJO_0             (T2SC1 &= ~TSCx_BIT_CHxMAX)

#define TIM2_CNL1_MASC_MODO                   (TSCx_BIT_MSxA|TSCx_BIT_ELSxB|TSCx_BIT_ELSxA)

#define TIM2_CNL1_MODO_PTO_ALTO               (T2SC1 = ((T2SC1)&(~TIM1_CNL1_MASC_MODO))|(0b000<<2))
#define TIM2_CNL1_MODO_PTO_BAJO               (T2SC1 = ((T2SC1)&(~TIM1_CNL1_MASC_MODO))|(0b100<<2))
#define TIM2_CNL1_MODO_ENT_FLANCO_ALTO        (T2SC1 = ((T2SC1)&(~TIM1_CNL1_MASC_MODO))|(0b001<<2))
#define TIM2_CNL1_MODO_ENT_FLANCO_BAJO        (T2SC1 = ((T2SC1)&(~TIM1_CNL1_MASC_MODO))|(0b010<<2))
#define TIM2_CNL1_MODO_ENT_FLANCO_AMBOS       (T2SC1 = ((T2SC1)&(~TIM1_CNL1_MASC_MODO))|(0b011<<2))
#define TIM2_CNL1_MODO_SALNOBUF_CAMBIAR_COMP  (T2SC1 = ((T2SC1)&(~TIM1_CNL1_MASC_MODO))|(0b101<<2))
#define TIM2_CNL1_MODO_SALNOBUF_CERO_COMP     (T2SC1 = ((T2SC1)&(~TIM1_CNL1_MASC_MODO))|(0b110<<2))
#define TIM2_CNL1_MODO_SALNOBUF_UNO_COMP      (T2SC1 = ((T2SC1)&(~TIM1_CNL1_MASC_MODO))|(0b111<<2))
#define TIM2_CNL1_MODO_PWMNOBUF_CAMBIAR_COMP  (T2SC1 = ((T2SC1)&(~TIM1_CNL1_MASC_MODO))|(0b101<<2))
#define TIM2_CNL1_MODO_PWMNOBUF_CERO_COMP     (T2SC1 = ((T2SC1)&(~TIM1_CNL1_MASC_MODO))|(0b110<<2))
#define TIM2_CNL1_MODO_PWMNOBUF_UNO_COMP      (T2SC1 = ((T2SC1)&(~TIM1_CNL1_MASC_MODO))|(0b111<<2))

/* Esta es la estructura del microcontrolador */

typedef struct s_mcu {
  ADC ModuloADC;
  IRQ ModuloIRQ;
  SCI ModuloSCI;
  TIM ModuloTIM1;
  TIM ModuloTIM2;
} MCU;

#endif // _HARDWARE_H_