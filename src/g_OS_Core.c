/*
 * stubs.c
 *
 *  Created on: 10 mar. 2018
 *      Author: gonza
 */

#include "g_OS_Core.h"

static task *g_spTarea_actual = NULL;
static task *g_spPrimera_tarea = NULL;

void init_task(void *tarea, task *tarea_struct, uint8_t prioridad)  {

	static uint8_t id = 0;						//el id sera correlativo a medida que se generen mas tareas
	static task *tarea_anterior;		//este sera el link para encadenar la tarea anterior con la actual

										//la funcion start_os recorre la lista hasta que encuentra un NULL y entonces
										//enlaza la ultima funcion a la primera


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

	tarea_struct->entry_point = tarea;			//Guardamos el entry point (solo a efectos de visualizacion en debugger)
	tarea_struct->task_id = id;					//le asignamos id										//la proxima tarea a definir tendra id+1

	tarea_struct->prioridad = prioridad;		//seteamos la prioridad (va a ser utilizado mas adelante)

	if (id > 0)  {
		//si estamos en la definicion de otra tarea que no sea la primera
		//tenemos el puntero a la estructura de la tarea anterior, por lo que el enlace debe hacerse a esta tarea
		tarea_anterior->tarea_siguiente = tarea_struct;
	}

	tarea_struct->tarea_siguiente = NULL;		//no se sabe si habra otra tarea luego a priori
	tarea_anterior = tarea_struct;				//antes de salir, dejamos el puntero a la estructura actual guardado

	id++;		//incrementamos el id para la proxima llamada

}

void start_os(task *primera_tarea)  {
	task *lista_enlazada;
	/*
	 * Todas las interrupciones tienen prioridad 0 (la maxima)
	 * al iniciar la ejecucion. Para que PendSV no interrumpa a Systick
	 * debemos bajarle la prioridad en el NVIC. La magia de la cuenta
	 * matematica da la probabilidad mas baja posible
	 */
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS)-1);

	//se recorre toda la lista enlazada para que la ultima tarea tenga como tarea siguiente a la primer tarea
	lista_enlazada = primera_tarea;
	while (lista_enlazada->tarea_siguiente != NULL)
		lista_enlazada = lista_enlazada->tarea_siguiente;

	lista_enlazada->tarea_siguiente = primera_tarea;	//la ultima tarea tendra como tarea siguiente la primer tarea

	g_spPrimera_tarea = primera_tarea;					//guardamos el valor de la primer tarea al iniciar el sistema operativo

}


uint32_t getNextContext(uint32_t sp_actual)  {

	uint32_t sp_next;


	if (g_spTarea_actual == NULL)  {
		sp_next = g_spPrimera_tarea->sp;		//al iniciar el OS, la primer PendSV carga el sp de la primer tarea
		g_spTarea_actual = g_spPrimera_tarea;	//y decimos que nuestra tarea actual es la primer tarea
	}
	else  {
		g_spTarea_actual->sp = sp_actual;						//guardamos el sp
		g_spTarea_actual = g_spTarea_actual->tarea_siguiente;	//apuntamos a la siguiente tarea
		sp_next = g_spTarea_actual->sp;							//cargamos el sp correspondiente
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
