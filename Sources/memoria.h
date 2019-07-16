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
* ARCHIVO: memoria.h                                                         *
*                                                                            *
*                                                                            *
* Alejandro Maita                                                            *
* Andres Arguello                                                            *
* Bruno Pierucci                                                             *
* Daniel Bello							                                                 *
* Nestor Bohorquez        						                                       *
*                                             Diciembre 2007 - Febrero 2008  *
******************************************************************************
*/

#ifndef _MEMORIA_H_
#define _MEMORIA_H_

/*
******************************************************************************
* 	                            ESPACIOS DE MEMORIA                          *
******************************************************************************
*/

#pragma DATA_SEG DATOS___P0
#pragma DATA_SEG DATOS___P1
#pragma DATA_SEG DATOS___P2
#pragma DATA_SEG DATOS___P3
#pragma DATA_SEG DATOS___P4
#pragma DATA_SEG DATOS___P5
#pragma DATA_SEG DEFAULT

#pragma CODE_SEG CODIGO__P0
#pragma CODE_SEG CODIGO__P1
#pragma CODE_SEG CODIGO__P2
#pragma CODE_SEG CODIGO__P3
#pragma CODE_SEG CODIGO__P4
#pragma CODE_SEG CODIGO__P5
#pragma CODE_SEG CODIGO__DEMONIOS
#pragma CODE_SEG DEFAULT

/* 
   Esta seccion es importante, define el punto de inicio del codigo de cada 
   proceso y el punto donde comienza la pila
*/

#define KERNEL_RAM      0x00F5     // dice RAM pero es mas bien la pila del proceso, acordate que la pila crece hacia abajo
#define KERNEL_ROM      0x8000

#define PROC0__RAM      0x012C
#define PROC0__ROM      0xA000

#define PROC1__RAM      0x0163
#define PROC1__ROM      0xA100     

#define PROC2__RAM      0x019A
#define PROC2__ROM      0xA200

#define PROC3__RAM      0x01D1
#define PROC3__ROM      0xA300

#define PROC4__RAM      0x0208
#define PROC4__ROM      0xA400

#define PROC5__RAM      0x023F
#define PROC5__ROM      0xA500

#define DEMONIOS_ROM    0xA700
    
#endif // _MEMORIA_H_