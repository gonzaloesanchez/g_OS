/*
 * g_OS_API.c
 *
 *  Created on: 10 abr. 2018
 *      Author: gonza
 */


#include "g_OS_API.h"


void osDelay(task* caller, uint32_t ticks)  {

	caller->ticks_bloqueada = ticks;		//cantidad de ticks a bloquearse
	caller->estado = TAREA_BLOCKED;			//estado de la tarea actual
	cpu_yield();							//ya no necesito la CPU hasta que el delay se termine
}
