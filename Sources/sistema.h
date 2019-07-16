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
* ARCHIVO: sistema.h                                                         *
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

#ifndef _SISTEMA_H_
#define _SISTEMA_H_

/*
******************************************************************************
* 				                     LLAMADAS AL SISTEMA                           *
******************************************************************************
*/  

// Llamadas para el despachador
#define DESP_REINICIAR              0xFD  // Cede la ejecucion al primer proceso en la cola
#define DESP_SIGUIENTE              0xFE     
#define NADA                        0xFF  // La llamada mas inutil de todas, no hace nada!!! XD

// Llamadas para el ADC
#define ADC_LEER_CNL0               0x00
#define ADC_LEER_CNL1               0x01
#define ADC_LEER_CNL2               0x02
#define ADC_LEER_CNL3               0x03
#define ADC_LEER_CNL4               0x04
#define ADC_LEER_CNL5               0x05
#define ADC_LEER_CNL6               0x06
#define ADC_LEER_CNL7               0x07

/* Llamadas para el SCI */
#define SCI_ESTABLECER_BUFFER       0x08

/* Llamadas para el IRQ */
#define IRQ_BLOQUEARSE              0x09

/* Llamadas para el TIM1 */
#define TIM1_BLOQUEARSE                       0x0A

#define TIM1_CNL0_ESPERAR_PRIMER_PULSO        0x0B
#define TIM1_CNL0_ESPERAR_SEGUNDO_PULSO       0x0C
#define TIM1_CNL0_LEER_ESTADO                 0x0D
#define TIM1_CNL0_LEER_CONTADOR_H             0x0E
#define TIM1_CNL0_LEER_CONTADOR_L             0x0F

#define TIM1_CNL1_ESPERAR_PRIMER_PULSO        0x10
#define TIM1_CNL1_ESPERAR_SEGUNDO_PULSO       0x11
#define TIM1_CNL1_LEER_ESTADO                 0x12
#define TIM1_CNL1_LEER_CONTADOR_H             0x13
#define TIM1_CNL1_LEER_CONTADOR_L             0x14

/* Llamadas para el TIM2 */
#define TIM2_BLOQUEARSE                       0x15

#define TIM2_CNL0_ESPERAR_PRIMER_PULSO        0x16
#define TIM2_CNL0_ESPERAR_SEGUNDO_PULSO       0x17
#define TIM2_CNL0_LEER_ESTADO                 0x18
#define TIM2_CNL0_LEER_CONTADOR_H             0x19
#define TIM2_CNL0_LEER_CONTADOR_L             0x1A

#define TIM2_CNL1_ESPERAR_PRIMER_PULSO        0x1B
#define TIM2_CNL1_ESPERAR_SEGUNDO_PULSO       0x1C
#define TIM2_CNL1_LEER_ESTADO                 0x1D
#define TIM2_CNL1_LEER_CONTADOR_H             0x1E
#define TIM2_CNL1_LEER_CONTADOR_L             0x1F

#endif // _SISTEMA_H_