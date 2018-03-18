/*
 * SysTick_Handler.S
 *
 *  Created on: 14/3/2018
 *      Author: gonza
 */

.syntax unified
.thumb

.data

	state: .word 0		//variable necesaria para el systick, inicializada en cero

.text
	.global SysTick_Handler
	.extern sp1,sp2,sistick_ms
	.thumb_func

SysTick_Handler:
	//sistick_ms++
	ldr r0,=sistick_ms
	ldr r1,[r0]
	add r1,1
	str r1,[r0]

	ldr r0, =state		// r0 = &state
	ldr r1, [r0]		//r1 = *r0
	tbb [pc,r1]
tabla:
	.byte (case0-tabla)/2
	.byte (case1-tabla)/2
	.byte (case2-tabla)/2
	.byte (case_end-tabla)/2

case0:
	//state = 1
	mov r1,1
	str r1,[r0]

	//sp = sp1
	ldr r0,=sp1
	ldr r1,[r0]
	msr msp,r1		//pisamos el MSP, perdimos el valor original y por lo tanto
					//los valores de R0-R3, R12, PC, LR y xPSR

	pop {R4-R11}
	/**
	*	el MSP esta apuntando al sp1, y el init task cargo una pila de 16 registros
	*   debemos hacer el pop de los 8 que no se hacen automaticos para que el sp1 quede
	*	alineado a los que hacen unstacking de forma automatica
	*   esto en realidad es basura
	*
	*	El primer llamado de cada tarea tendra un valor basura en sus registros R4-R11
	*	dado que estamos haciendo pop de valores de Stack no inicializados.
	*	Luego en sucesivas interrupciones, estos valores tendran sentido
	*/

	//break;
	b case_end

case1:
	//state = 2
	mov r1,2
	str r1,[r0]

	push {R4-R11}	//el MSP esta apuntando al sp1, por lo que hacemos correr la pila (+8)

	//sp1 = sp
	ldr r0,=sp1
	mrs r1,msp
	str r1,[r0]

	//sp = sp2
	ldr r0,=sp2
	ldr r1,[r0]
	msr msp,r1

	pop {R4-R11}	//ahora apuntando al sp2 hacemos pop y devolvemos los valores de los registros
					//antes de volver a la tarea 2
	//break;
	b case_end

case2:
	//state = 1
	mov r1,1
	str r1,[r0]

	push {R4-R11}	//el MSP esta apuntando al sp2, por lo que hacemos correr la pila

	//sp2 = sp
	ldr r0,=sp2
	mrs r1,msp
	str r1,[r0]
	//sp = sp1
	ldr r0,=sp1
	ldr r1,[r0]
	msr msp,r1

	pop {R4-R11}	//ahora apuntando al sp1 hacemos pop y devolvemos los valores de los registros
					//antes de volver a la tarea 1

case_end:
	bx lr			//carga en PC el valor de EXEC_RETURN para que se vuelva donde apunta MSP (en este caso. Es segun este armada la pila)


