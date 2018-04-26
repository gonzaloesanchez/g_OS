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


/********************************************************************************
 * Funcion Delay para las tareas
 *******************************************************************************/
void os_Delay(uint32_t ticks)  {

	/*
	 * Cargamos la cantidad de ticks a la tarea actual que se esta corriendo si la misma esta en running
	 * y si los ticks son mayores a cero
	 */
	if (g_sControl_OS.spTarea_actual->estado == TAREA_RUNNING &&
		ticks > 0)  {

		os_enter_critical();

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		g_sControl_OS.spTarea_actual->ticks_bloqueada = ticks;		//cantidad de ticks a bloquearse
		g_sControl_OS.spTarea_actual->estado = TAREA_BLOCKED;
		//------------------------------------------------------------------------------------------

		os_exit_critical();


		/*
		 * Si la tarea fue puesta en ready por otro evento, debemos volverla a bloquear, porque
		 * los ticks aun no terminaron.
		 */

		while (g_sControl_OS.spTarea_actual->ticks_bloqueada > 0)  {
			g_sControl_OS.spTarea_actual->estado = TAREA_BLOCKED;
			cpu_yield();              //ya no necesito la CPU hasta que el delay se termine
		}
	}
}
//-------------------------------------------------------------------------------



/********************************************************************************
 * Funcion de inicializacion para los semaforos. Todos se inician tomados
 *******************************************************************************/
void os_Semaforo_init(osSemaforo* X)  {
	X->tomado = true;
	X->tarea_asociada = NULL;
}


//-------------------------------------------------------------------------------

/********************************************************************************
 * Funcion de tomar (take) para los semaforos
 * Argumentos:
 * 		@osSemaforo* X		Es el semaforo que se quiere utilizar

 *******************************************************************************/
void os_Semaforo_take(osSemaforo* X)  {
	bool Salir = false;

	if (g_sControl_OS.spTarea_actual->estado == TAREA_RUNNING)  {
		/*
		 * Esto lo puse dentro de un while porque la tarea puede desbloquearse por otro
		 * evento, pero debe volver a bloquearse si no fue el semaforo quien la desbloqueo
		 */
		while (!Salir)  {

			if(X->tomado)  {
				os_enter_critical();

				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				g_sControl_OS.spTarea_actual->estado = TAREA_BLOCKED;		//estado de la tarea actual
				X->tarea_asociada = g_sControl_OS.spTarea_actual;			//guardamos la tarea que bloqueamos
				//----------------------------------------------------------------------------------------------------

				os_exit_critical();
				cpu_yield();			//devolvemos el CPU
			}
			else  {
				X->tomado = true;
				Salir = true;
			}
		}
	}
}



//-------------------------------------------------------------------------------

/********************************************************************************
 * Funcion de liberar (give) para los semaforos
 *******************************************************************************/
void os_Semaforo_give(osSemaforo* X)  {

	if (g_sControl_OS.spTarea_actual->estado == TAREA_RUNNING &&
		X->tomado == true && X->tarea_asociada != NULL)  {

		X->tomado = false;
		X->tarea_asociada->estado = TAREA_READY;		//liberamos la tarea que habiamos bloqueado
	}
}

//-------------------------------------------------------------------------------
