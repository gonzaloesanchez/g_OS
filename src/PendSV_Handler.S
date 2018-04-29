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

// !!!!!!!!!!!!!!!!!! seccion critica !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	cpsid i				//disable interrupts (Es como si fuera el GIE)

//FPU Context
	tst lr,0x10			//bit 4 del EXEC_RETURN es cero?
	it eq
	vpusheq {s16-s31}
								//el push se hace al reves de como esta escrito!!!
	push {r4-r11,lr}			//debemos guardar el valor de EXEC_RETURN porque sino
	mrs r0,msp					//al llamar a getNextContext pisamos el valor de LR
	bl getNextContext
	msr msp,r0
	pop {r4-r11,lr}		//ademas tenemos todos los registros guardados

	tst lr,0x10			//bit 4 del EXEC_RETURN es cero?
	it eq
	vpopeq {s16-s31}	//hacemos POP de los registros

// ------------------ Fin de la seccion critica -----------------------------------------
	cpsie i				//enable interrupts (Como si fuera el GIE)

	bx lr				//carga en PC el valor de EXEC_RETURN para que se vuelva donde apunta MSP
						//(en este caso. Es segun este armada la pila)


