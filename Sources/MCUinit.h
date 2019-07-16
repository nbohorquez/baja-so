/*
** ###################################################################
**     THIS MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Project   : SistemaOperativo
**     Processor : MC68HC908GP32CP
**     Version   : Bean 01.108, Driver 01.00, CPU db: 2.89.043
**     Datasheet : MC68HC908GP32 Rev. 6.1 08/2005
**     Date/Time : 15/02/2008, 03:11 a.m.
**     Abstract  :
**         This bean "MC68HC908GP32_40" provides initialization of the 
**         CPU core and shared peripherals.
**     Settings  :
**         Clock setting
**             External clock      : 0.032768 MHz
**             CPU mode selection  : 1
**             Initialization interrupt priority : 0
**             Stop instruction enabled : no
**             LVI module          : yes
**
**             Source clock        : Main clock frequency / 1
**             Internal bus clock  : 7.3728 MHz
**     Contents  :
**         Function "MCU_init" initializes selected peripherals
**
**     (c) Copyright UNIS, spol. s r.o. 1997-2005
**     UNIS, spol s r.o.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################
*/

#ifndef __SistemaOperativo_H
#define __SistemaOperativo_H 1

/* Include shared modules, which are used for whole project */

extern void MCU_init(void);
/*
** ===================================================================
**     Method      :  MCU_init (bean MC68HC908GP32_40)
**
**     Description :
**         Device initialization code for selected peripherals.
** ===================================================================
*/

__interrupt void isrINT_TBM(void);
/*
** ===================================================================
**     Interrupt handler : isrINT_TBM
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



__interrupt void isrINT_ADC(void);
/*
** ===================================================================
**     Interrupt handler : isrINT_ADC
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



__interrupt void isrINT_KBD(void);
/*
** ===================================================================
**     Interrupt handler : isrINT_KBD
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



__interrupt void isrINT_SCITransmit(void);
/*
** ===================================================================
**     Interrupt handler : isrINT_SCITransmit
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



__interrupt void isrINT_SCIReceive(void);
/*
** ===================================================================
**     Interrupt handler : isrINT_SCIReceive
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



__interrupt void isrINT_SCIError(void);
/*
** ===================================================================
**     Interrupt handler : isrINT_SCIError
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



__interrupt void isrINT_TIM2Ovr(void);
/*
** ===================================================================
**     Interrupt handler : isrINT_TIM2Ovr
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



__interrupt void isrINT_TIM1Ovr(void);
/*
** ===================================================================
**     Interrupt handler : isrINT_TIM1Ovr
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



__interrupt void isrINT_TIM1CH1(void);
/*
** ===================================================================
**     Interrupt handler : isrINT_TIM1CH1
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



__interrupt void isrINT_IRQ(void);
/*
** ===================================================================
**     Interrupt handler : isrINT_IRQ
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



__interrupt void isrINT_SWI(void);
/*
** ===================================================================
**     Interrupt handler : isrINT_IRQ
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/


/* END SistemaOperativo */

#endif
/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.97.01 [03.73]
**     for the Freescale HC08 series of microcontrollers.
**
** ###################################################################
*/
