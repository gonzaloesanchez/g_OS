/*
 * stubs.c
 *
 *  Created on: 10 mar. 2018
 *      Author: gonza
 */

#include "g_OS_Core.h"

/*
 * Definiciones necesarias para el funcionamiento del Sistema Operativo
 */
static osControl g_sControl_OS;
task g_sIdleTask;					//definicion de contexto para tarea IDLE

/********************************************************************************
 * Definiciones de funciones que el usuario puede poblar con codigo
 *******************************************************************************/

__WEAK__ void ReturnHook(void)  {
	while(1);
}

__WEAK__ void taskIdle(void)  {
	__WFI();
}

__WEAK__ void ErrorHook(void *Caller)  {
	/*
	 * Revisar el contenido de g_Error para obtener informacion!!
	 */
	while(1);
}

//-------------------------------------------------------------------------------

/********************************************************************************
 * Init Task
 *******************************************************************************/
void init_task(void *tarea, task *tarea_struct, uint8_t prioridad)  {

	static uint8_t id = 0;				//el id sera correlativo a medida que se generen mas tareas

	/*
	 * Se hace una especie de ASSERT donde determinamos que la cantidad de tareas no exceda la longitud
	 * del vector definido
	 */
	if(id < MAX_TASK_NUM-1)  {

		memset(tarea_struct->pila,0,STACK_SIZE);		//El stack a cero!!!
		/*
		 * Movemos la posicion del sp guardando el espacio para el stack de la tarea
		 */
		tarea_struct->sp = (uint32_t)(tarea_struct->pila + STACK_SIZE/4 - FULL_STACKING_SIZE);

		/*
		 * Armamos el stack
		 */
		tarea_struct->pila[STACK_SIZE/4 - PSR] = INIT_xPSR;					//psr.t = 1
		tarea_struct->pila[STACK_SIZE/4 - PC_REG] = (uint32_t)tarea;		//direccion de la tarea (ENTRY_POINT)
		tarea_struct->pila[STACK_SIZE/4 - LR] = (uint32_t)ReturnHook;		//Retorno de la tarea (no deberia darse)
		tarea_struct->pila[STACK_SIZE/4 - LR_PREV_VALUE] = EXEC_RETURN;

		tarea_struct->entry_point = tarea;			//Guardamos el entry point
		tarea_struct->task_id = id;					//le asignamos id, la proxima tarea a definir tendra id+1
		tarea_struct->prioridad = prioridad;		//seteamos la prioridad (va a ser utilizado mas adelante)
		tarea_struct->estado = TAREA_READY;			//todas las tareas se crean en estado READY

		g_sControl_OS.ListaTareas[id] = tarea_struct;			//guardamos el puntero a la tarea que acabamos de definir
		g_sControl_OS.cantidad_Tareas = id+1;					//nos va a dar la cantidad de tareas definidas (minimo = 2)

		id++;		//incrementamos el id para la proxima llamada
	}
	else  {
		g_sControl_OS.Error = ERR_OS_CANT_TAREAS;		//excedimos la cantidad de tareas posibles
		ErrorHook(init_task);				//llamamos a error hook dando informacion de quien lo esta llamando
	}
}
//-------------------------------------------------------------------------------


/********************************************************************************
 * Start OS
 *******************************************************************************/
void start_os()  {
	uint8_t i;
	/*
	 * Todas las interrupciones tienen prioridad 0 (la maxima)
	 * al iniciar la ejecucion. Para que PendSV no interrumpa a Systick
	 * debemos bajarle la prioridad en el NVIC. La magia de la cuenta
	 * matematica da la probabilidad mas baja posible
	 */
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS)-1);

	/**
	 * es necesaria la inicializacion de nuestra idle task, y esta NO ES VISIBLE al usuario
	 * El usuario puede eventualmente poblarla de codigo o redefinirla, pero no debe cambiar el
	 * codigo ni definir una estructura para la misma.
	 * Al ser definida aqui, queda enlazada en ultimo lugar
	 */

	init_task(taskIdle,&g_sIdleTask,0);

	/*
	 * Terminamos de inicializar el vector de tareas asegurandonos que los valores despues de
	 * IDLE sean NULL
	 */
	g_sControl_OS.bStartOS = true;
	g_sControl_OS.spTarea_actual = NULL;			//solamente el scheduler determina
	g_sControl_OS.spTarea_siguiente = NULL;			//que tarea inicia y cual le sigue

	for (i=0;i<MAX_TASK_NUM;i++)  {
		if(i>=g_sControl_OS.cantidad_Tareas)
			g_sControl_OS.ListaTareas[i] = NULL;	//luego de la tarea IDLE tenemos que poner todas en NULL
	}
}
//-------------------------------------------------------------------------------

/********************************************************************************
 * Get Next Context
 *******************************************************************************/
uint32_t getNextContext(uint32_t sp_actual)  {

	uint32_t sp_next;

	/*
	 * Aqui cabe la aclaracion de que TODA LA MAGIA de tarea actual y siguiente sucede en el scheduler
	 * Esto nos da generalidad para poder cargar tareas por prioridad o irnos a IDLE sin tener que volver
	 * a tocar esta funcion
	 */

	/**
	 * TODO:
	 * 		1) La tarea actual debe ponerse en READY (done)
	 * 		2) La tarea siguiente en RUNNING (done)
	 */

	if (g_sControl_OS.bStartOS == true)  {
		g_sControl_OS.spTarea_actual = (task*) g_sControl_OS.ListaTareas[0];	//cargamos el puntero de la primer tarea
		sp_next = g_sControl_OS.spTarea_actual->sp;		//al iniciar el OS, la primer PendSV carga el sp de la primer tarea
														//(es el inicio, esta en ready SEGURO)
		g_sControl_OS.spTarea_actual->estado = TAREA_RUNNING;	//ponemos el estado de la primera tarea en running
		g_sControl_OS.bStartOS = false;						//esta condicion no se da mas
	}
	else {
		g_sControl_OS.spTarea_actual->sp = sp_actual;		//guardamos el sp de la tarea actualmente en ejecucion (RUNNING)
		/*
		 * Solamente puedo estar seguro de pasar a READY si la tarea llego aqui en running
		 * Porque si alguna API cambio a BLOCKED no puedo pasarla a READY en est
		 */
		if (g_sControl_OS.spTarea_actual->estado == TAREA_RUNNING)
			g_sControl_OS.spTarea_actual->estado = TAREA_READY;

		sp_next = g_sControl_OS.spTarea_siguiente->sp;						//cargamos el sp correspondiente

		g_sControl_OS.spTarea_actual = g_sControl_OS.spTarea_siguiente;		//actualizamos la tarea actual
		g_sControl_OS.spTarea_actual->estado = TAREA_RUNNING;				//esta tarea paso a estado RUNNING
	}

	return sp_next;
}
//-------------------------------------------------------------------------------

/********************************************************************************
 * Scheduler (si hace falta lo escribo en ASM
 *******************************************************************************/
void scheduler(void)  {
	/**
	 * TODO:
	 * 		1) El scheduler tiene que determinar cual es la tarea siguiente
	 * 		2) Si todas las tareas estan en blocked, la tarea siguiente sera IDLE
	 */
	uint8_t indice;		//variable auxiliar para legibilidad

	/*
	 * Obtenemos la siguiente tarea en el vector
	 */
	indice = g_sControl_OS.spTarea_actual->task_id+1;
	/*
	 * Excluimos a la tarea IDLE. Si todavia quedan tareas, la tarea siguiente es la que sigue en el
	 * vector lista de tareas. Sino volvemos a apuntar a la tarea 0 que es la primera
	 */
	if(indice < g_sControl_OS.cantidad_Tareas-1)  {
		g_sControl_OS.spTarea_siguiente = (task*) g_sControl_OS.ListaTareas[indice];
	}
	else  {
		g_sControl_OS.spTarea_siguiente = (task*) g_sControl_OS.ListaTareas[0];
	}

}
//-------------------------------------------------------------------------------

void SysTick_Handler(void)  {

	scheduler();			//scheduler debe ser LO MAS CORTO POSIBLE

	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

	/* Instruction Synchronization Barrier: aseguramos que se
	 * ejecuten todas las instrucciones en  el pipeline
	 */
	__ISB();

	/* Data Synchronization Barrier: aseguramos que se
	 * completen todos los accesos a memoria
	 */
	__DSB();
}
