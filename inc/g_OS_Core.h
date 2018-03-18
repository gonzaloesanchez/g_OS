/*
 * stubs.h
 *
 *  Created on: 10 mar. 2018
 *      Author: gonza
 */


#ifndef SIST_OPERATIVOS1_G_OS_INC_STUBS_H_
#define SIST_OPERATIVOS1_G_OS_INC_STUBS_H_

#include <stdint.h>

#define STACK_SIZE 128
#define AUTO_STACKING_SIZE 8	// registros que hacen stacking automatico, sin contar Floating point registers
#define FULL_STACKING_SIZE 16	//Todos los regstros excepto FPunit

//posiciones de registros de automatic stacking
#define PSR		1
#define PC_REG	2
#define LR		3
#define R12		4
#define R3		5
#define R2		6
#define R1		7
#define R0		8

//posiciones de registros restantes
#define R4	9
#define R5	10
#define R6	11
#define R7	12
#define R8	13
#define R9	14
#define R10 15
#define R11 16




void init_task(void *task, uint32_t *stack, uint32_t *stack_pointer);
void start_os(void);
void ReturnHook(void);


#endif /* SIST_OPERATIVOS1_G_OS_INC_STUBS_H_ */
