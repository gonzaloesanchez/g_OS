/*
 * PendSV_Handler.S
 *
 *  Created on: 14/3/2018
 *      Author: gonza
 */

.syntax unified
.thumb

.data

	state: .word 0		//variable necesaria para el systick, inicializada en cero

.text
	.global PendSV_Handler
	.extern getNextContext
	.thumb_func

PendSV_Handler:
								//el push se hace al reves de como esta escrito!!!
	push {r4-r11,lr}			//debemos guardar el valor de EXEC_RETURN porque sino
	mrs r0,msp					//al llamar a getNextContext pisamos el valor de LR
	bl getNextContext
	msr msp,r0
	pop {r4-r11,lr}		//ademas tenemos todos los registros guardados
	bx lr				//carga en PC el valor de EXEC_RETURN para que se vuelva donde apunta MSP
						//(en este caso. Es segun este armada la pila)


