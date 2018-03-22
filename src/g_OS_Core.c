/*
 * stubs.c
 *
 *  Created on: 10 mar. 2018
 *      Author: gonza
 */

#include "g_OS_Core.h"

void init_task(void *task, uint32_t *stack, uint32_t *stack_pointer)  {

	memset(stack,0,STACK_SIZE);		//El stack a cero!!!
	/*
	 * Movemos la posicion del sp guardando el espacio para el stack de la tarea
	 */
	*stack_pointer = (uint32_t)(stack + STACK_SIZE/4 - FULL_STACKING_SIZE);

	/*
	 * Armamos el stack
	 */
	stack[STACK_SIZE/4 - PSR] = INIT_xPSR;				//psr.t = 1
	stack[STACK_SIZE/4 - PC_REG] = (uint32_t)task;		//direccion de la tarea (ENTRY_POINT)
	stack[STACK_SIZE/4 - LR] = (uint32_t)ReturnHook;	//Retorno de la tarea (no deberia darse)
	stack[STACK_SIZE/4 - LR_PREV_VALUE] = EXEC_RETURN;


}

void start_os(void)  {
	/*
	 * Todas las interrupciones tienen prioridad 0 (la maxima)
	 * al iniciar la ejecucion. Para que PendSV no interrumpa a Systick
	 * debemos bajarle la prioridad en el NVIC. La magia de la cuenta
	 * matematica da la probabilidad mas baja posible
	 */
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS)-1);

}


uint32_t getNextContext(uint32_t sp_actual)  {
	static int actual_task = -1;
	uint32_t sp_next;

	switch(actual_task)  {

	case 1:
		sp1 = sp_actual;
		sp_next = sp2;
		actual_task = 2;
		break;

	case 2:
		sp2 = sp_actual;
		sp_next = sp1;
		actual_task = 1;
		break;

	default:
		sp_next = sp1;
		actual_task = 1;
		break;
	}

	return sp_next;
}

void SysTick_Handler(void)  {

	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;


	__ISB();

	__DSB();
}


void ReturnHook(void)  {
	while(1);
}
