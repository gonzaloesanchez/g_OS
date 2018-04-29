/*
 * g_OS_IRQ.h
 *
 *  Created on: 29 abr. 2018
 *      Author: gonza
 */

#ifndef G_OS_INC_G_OS_IRQ_H_
#define G_OS_INC_G_OS_IRQ_H_


#include "g_OS_Core.h"
#include "g_OS_API.h"
#include "board.h"
#include "cmsis_43xx.h"

#define CANT_IRQ	53

extern osControl g_sControl_OS;

bool os_install_IRQ(LPC43XX_IRQn_Type irq, void* usr_irq);
bool os_remove_IRQ(LPC43XX_IRQn_Type irq);


#endif /* G_OS_INC_G_OS_IRQ_H_ */
