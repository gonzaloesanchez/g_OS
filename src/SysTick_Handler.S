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
	.extern sp1,sp2
	.thumb_func

SysTick_Handler:
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
	msr msp,r1
	//break;
	b case_end

case1:
	//state = 2
	mov r1,2
	str r1,[r0]

	//sp1 = sp
	ldr r0,=sp1
	mrs r1,msp
	str r1,[r0]
	//sp = sp2
	ldr r0,=sp2
	ldr r1,[r0]
	msr msp,r1
	//break;
	b case_end

case2:
	//state = 1
	mov r1,1
	str r1,[r0]

	//sp2 = sp
	ldr r0,=sp2
	mrs r1,msp
	str r1,[r0]
	//sp = sp1
	ldr r0,=sp1
	ldr r1,[r0]
	msr msp,r1

case_end:
	bx lr			//volvemos a donde esta apuntado en LR


