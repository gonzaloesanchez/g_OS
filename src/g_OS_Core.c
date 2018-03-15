/*
 * stubs.c
 *
 *  Created on: 10 mar. 2018
 *      Author: gonza
 */

#include "g_OS_Core.h"

void init_task(void *task, uint32_t *stack, uint32_t *stack_pointer)  {
	*stack_pointer = (uint32_t)(stack+STACK_SIZE/4 - 8);
	stack[STACK_SIZE/4 - 1] = 1 << 24;			//psr.t = 1
	stack[STACK_SIZE/4 - 2] = (uint32_t)task;		//direccion de la tarea


}

void start_os(void)  {

}
