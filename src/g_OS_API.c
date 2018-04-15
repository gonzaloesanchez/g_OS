/*
 * g_OS_API.c
 *
 *  Created on: 10 abr. 2018
 *      Author: gonza
 */


#include "g_OS_API.h"


/********************************************************************************
 * Definiciones de funciones que el usuario puede poblar con codigo
 *******************************************************************************/

__WEAK__ void ReturnHook(void)  {
	while(1);
}

__WEAK__ void taskIdle(void)  {
	while(1)  {
		__WFI();
	}
}

__WEAK__ void ErrorHook(void *Caller)  {
	/*
	 * Revisar el contenido de g_Error para obtener informacion!!
	 */
	while(1);
}

//-------------------------------------------------------------------------------




void os_Delay(uint32_t ticks)  {

	/*
	 * Cargamos la cantidad de ticks a la tarea actual que se esta corriendo si la misma esta en running
	 * y si los ticks son mayores a cero
	 */
	if (g_sControl_OS.spTarea_actual->estado == TAREA_RUNNING &&
		ticks > 0)  {

		g_sControl_OS.spTarea_actual->ticks_bloqueada = ticks;		//cantidad de ticks a bloquearse
		g_sControl_OS.spTarea_actual->estado = TAREA_BLOCKED;		//estado de la tarea actual
		cpu_yield();							//ya no necesito la CPU hasta que el delay se termine
	}
}
