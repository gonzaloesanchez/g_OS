/*
 * stubs.c
 *
 *  Created on: 10 mar. 2018
 *      Author: gonza
 */

#include "g_OS_Core.h"

void init_task(void *task, uint32_t *stack, uint32_t *stack_pointer)  {
	/*
	 * Movemos la posicion del sp guardando el espacio para el stack de la tarea
	 */
	*stack_pointer = (uint32_t)(stack + STACK_SIZE - FULL_STACKING_SIZE);

	/*
	 * Armamos el stack
	 */
	stack[STACK_SIZE - PSR] = 1 << 24;				//psr.t = 1
	stack[STACK_SIZE - PC_REG] = (uint32_t)task;			//direccion de la tarea (ENTRY_POINT)
	stack[STACK_SIZE - LR] = (uint32_t)ReturnHook;	//Retorno de la tarea (no deberia darse)



}

void start_os(void)  {

}


void ReturnHook(void)  {
	while(1);
}
