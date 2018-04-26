/*
 * stubs.c
 *
 *  Created on: 10 mar. 2018
 *      Author: gonza
 */

#include "g_OS_Core.h"
#include "g_OS_API.h"

/*
 * Definiciones necesarias para el funcionamiento del Sistema Operativo
 */
osControl g_sControl_OS;
static task g_sIdleTask;					//definicion de contexto para tarea IDLE
static uint8_t id[CANT_PRIORIDADES];		//esta variable me ayuda a seguir el track de IDs
static uint8_t indice[CANT_PRIORIDADES];	//variable necesaria para lograr el round robin
uint32_t g_ui32Ticks = 0;				//contador global



/********************************************************************************
 * Init OS. Solamente sirve para asegurarnos de que la memoria RAM asociada a la
 * estructura de control comience en 0. Es llamada por la primer init_task que se
 * llama
 *******************************************************************************/
static void init_mem(void)  {
	int i;
	/*
	 * Me aseguro de que toda la memoria asociada a las listas de tareas contenga NULL
	 * antes de iniciar
	 */
	for(i=0;i<CANT_PRIORIDADES;i++)  {
		memset(g_sControl_OS.ListaTareas[i],0,MAX_TASK_NUM * sizeof(void));		//cada lista de prioridad contiene todos NULL
	}
	memset(id,0,CANT_PRIORIDADES * sizeof(uint8_t));			//pongo a cero todos los ID
	memset(indice,0,CANT_PRIORIDADES * sizeof(uint8_t));		//pongo a cero todos los indices
	g_sControl_OS.contador_critico = 0;							//inicializo contador de entradas a secciones criticas
}
//-------------------------------------------------------------------------------




/********************************************************************************
 * Init Task
 *******************************************************************************/
void os_init_task(void *tarea, task *tarea_struct, uint8_t prioridad)  {

	/*
	 * Se hace una especie de ASSERT donde determinamos que la cantidad de tareas no exceda la longitud
	 * del vector definido
	 */
	if(id[prioridad] < MAX_TASK_NUM-1)  {

		/*
		 * Preguntamos si es la primer llamada a init task, esto es, si no hay ningun otra tarea definida
		 * y si es asi, inicializamos la memoria asociada a las estructuras del sistema operativo
		 */
		if (id[PRIORIDAD_0] == 0 &&
			id[PRIORIDAD_1] == 0 &&
			id[PRIORIDAD_2] == 0 &&
			id[PRIORIDAD_3] == 0 &&
			id[PRIORIDAD_4] == 0 &&
			id[PRIORIDAD_5] == 0 &&
			id[PRIORIDAD_6] == 0 &&
			id[PRIORIDAD_7] == 0 )  {

			init_mem();
		}


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
		tarea_struct->task_id = id[prioridad];					//le asignamos id, la proxima tarea a definir tendra id+1
		tarea_struct->prioridad = prioridad;		//seteamos la prioridad (va a ser utilizado mas adelante)
		tarea_struct->estado = TAREA_READY;			//todas las tareas se crean en estado READY
		tarea_struct->ticks_bloqueada = 0;			//inicializamos la variable que corresponde a bloqueos

		g_sControl_OS.ListaTareas[prioridad][id[prioridad]] = tarea_struct;		//guardamos el puntero a la tarea que acabamos de definir
		g_sControl_OS.cantidad_Tareas[prioridad] += 1;							//nos va a dar la cantidad de tareas definidas

		id[prioridad] += 1;		//incrementamos el id para la proxima llamada
	}
	else  {
		g_sControl_OS.Error = ERR_OS_CANT_TAREAS;		//excedimos la cantidad de tareas posibles
		ErrorHook(os_init_task);				//llamamos a error hook dando informacion de quien lo esta llamando
	}
}
//-------------------------------------------------------------------------------


/********************************************************************************
 * Start OS.
 *******************************************************************************/
void os_start()  {
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

	os_init_task(taskIdle,&g_sIdleTask,PRIORIDAD_IDLE);

	/*
	 * Terminamos de inicializar el vector de tareas asegurandonos que los valores despues de
	 * IDLE sean NULL
	 */
	g_sControl_OS.bStartOS = true;
	g_sControl_OS.spTarea_actual = NULL;			//solamente el scheduler determina
	g_sControl_OS.spTarea_siguiente = NULL;			//que tarea inicia y cual le sigue
}
//-------------------------------------------------------------------------------

/********************************************************************************
 * Scheduler (si hace falta lo escribo en ASM)
 *******************************************************************************/
static void scheduler(void)  {
	/**
	 * TODO:
	 * 		1) El scheduler tiene que determinar cual es la tarea siguiente (done)
	 * 		2) Si todas las tareas estan en blocked, la tarea siguiente sera IDLE (done)
	 * 		3) Round Robin para tareas en igualdad de prioridad (done)
	 */
	uint8_t prioridad;
	uint8_t cant_bloqueadas;
	bool Salir = false;
	task* tarea;		//variable auxiliar para legibilidad


	/*
	 * Recorremos todas las listas, empezando por la de mayor prioridad
	 * Si en esa prioridad hay tareas, entonces comenzamos a hacer round robin en
	 * esa lista de prioridad. Caso contrario, bajamos un escalon de prioridad
	 */
	prioridad = 0;
	cant_bloqueadas = 0;

	while (!Salir)  {
		if (g_sControl_OS.cantidad_Tareas[prioridad] > 0)  {

			/*
			 * Verificamos que el indice no exceda las tareas existentes
			 */
			if (indice[prioridad] > g_sControl_OS.cantidad_Tareas[prioridad]-1)
				indice[prioridad] = 0;
			/*
			 * Cargamos la tarea correspondiente a la lista de prioridad
			 */
			tarea = (task*) g_sControl_OS.ListaTareas[prioridad][indice[prioridad]];

			switch (tarea->estado)  {
			/*
			 * Este caso se da SOLO para una tarea unica en su lista, la siguiente tarea es ella
			 * misma por lo que al preguntar por la tarea siguiente, el resultado es que esta
			 * en estado RUNNING. Nada que hacer, hasta que no aparezca una tarea de mayor
			 * prioridad o se bloquee, debe seguir funcionando.
			 * En el caso de haber dos tareas en la misma lista de prioridad, debemos forzar a que
			 * el indice se incremente evitando salir del bucle.
			 */
			case TAREA_RUNNING:
				Salir = true;
				break;


			/*
			 * La tarea cargada esta en ready, y por la forma en que se recorren las listas, es la de
			 * mayor prioridad primero encontrada
			 */
			case TAREA_READY:
				g_sControl_OS.spTarea_siguiente = tarea;
				Salir = true;
				break;

			case TAREA_BLOCKED:
				cant_bloqueadas++;
				break;

			case TAREA_HALTED:
				break;
			default:
				;
			}

			/*
			 * Incrementamos el indice para que la proxima vez que se ingrese al bucle
			 * ya sea en la misma interrupcion o en la siguiente, se este apuntando a la
			 * siguiente tarea en la lista de prioridad
			 */
			indice[prioridad]++;

			/*
			 * Si la cantidad de tareas bloqueadas es igual al total de tareas definidas por usuario
			 * en esta lista, entonces debemos pasar a otra lista de menor prioridad
			 */

			if (cant_bloqueadas == g_sControl_OS.cantidad_Tareas[prioridad])  {
				/*
				 * No habia tareas en READY en la lista de prioridad, bajamos un escalon de prioridad
				 * Al no haber ninguna tarea disponible, el iterador i JAMAS deberia pasar la cantidad
				 * de prioidades, porque la tarea IDLE siempre esta en READY. Si ese caso se da, es un
				 * error interno del OS
				 */
				cant_bloqueadas = 0;		//reiniciamos el contador de bloquedas
				if(!(++prioridad < CANT_PRIORIDADES))  {
					g_sControl_OS.Error = ERR_PRIORITY_OVERFLOW;		//esto jamas deberia darse
					ErrorHook(scheduler);
				}
			}
		}


		else  {
			/*
			 * No habia tareas en esa lista de prioridad (cant = 0), bajamos un escalon de prioridad
			 * Al no haber ninguna tarea disponible, el iterador i JAMAS deberia pasar la cantidad
			 * de prioidades, porque la tarea IDLE siempre esta en READY. Si ese caso se da, es un
			 * error interno del OS
			 */

			if(!(++prioridad < CANT_PRIORIDADES))  {
				g_sControl_OS.Error = ERR_PRIORITY_OVERFLOW;		//esto jamas deberia darse
				ErrorHook(scheduler);
			}
		}
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
	 * a tocar get_next_context. Siempre que haya que hacer un cambio de contexto debemos haber pasado antes
	 * por el scheduler
	 */
	scheduler();

	/**
	 * TODO:
	 * 		1) La tarea actual debe ponerse en READY (done)
	 * 		2) La tarea siguiente en RUNNING (done)
	 */

	/*
	 * Como el Scheduler funciona bien y la primera vez que es llamado toma la primer tarea de mayor prioridad,
	 * controlo si el sistema operativo acaba de comenzar y si es asi, piso el puntero de mi tarea actual
	 * con el de la tarea siguiente, que no es mas que la primer tarea a ejecutar.
	 */
	if (g_sControl_OS.bStartOS)  {
		g_sControl_OS.spTarea_actual = g_sControl_OS.spTarea_siguiente;	//cargamos el puntero de la primer tarea
		sp_next = g_sControl_OS.spTarea_actual->sp;		//al iniciar el OS, la primer PendSV carga el sp de la primer tarea
		//(es el inicio, esta en ready SEGURO)
		g_sControl_OS.spTarea_actual->estado = TAREA_RUNNING;	//ponemos el estado de la primera tarea en running
		g_sControl_OS.bStartOS = false;						//esta condicion no se da mas
	}

	else  {
		g_sControl_OS.spTarea_actual->sp = sp_actual;		//guardamos el sp de la tarea actualmente en ejecucion (RUNNING)
		/*
		 * Solamente puedo estar seguro de pasar a READY si la tarea llego aqui en running
		 * Porque si alguna API cambio a BLOCKED no puedo pasarla a READY en est
		 */
		if (g_sControl_OS.spTarea_actual->estado == TAREA_RUNNING)
			g_sControl_OS.spTarea_actual->estado = TAREA_READY;



		/*
		 * cargamos el sp correspondiente
		 */
		sp_next = g_sControl_OS.spTarea_siguiente->sp;

		/*
		 * actualizamos el puntero a la tarea actual y la ponemos en running
		 */
		g_sControl_OS.spTarea_actual = g_sControl_OS.spTarea_siguiente;
		g_sControl_OS.spTarea_actual->estado = TAREA_RUNNING;
	}
	return sp_next;
}
//-------------------------------------------------------------------------------

void SysTick_Handler(void)  {
	uint8_t prioridad,i;
	task *tarea;

	/*
	 * Systick justamente es el que se encarga de ver todos los temporizadores
	 * por lo que recorremos todas las tareas que esten definidas y si tienen un
	 * valor de ticks de bloqueo mayor a cero, lo decrementamos.
	 *
	 */
	for (prioridad=0;prioridad<CANT_PRIORIDADES;prioridad++)  {
		if (g_sControl_OS.cantidad_Tareas[prioridad] > 0)  {
			/*
			 * Si existen tareas en esta lista de prioridad, las recorremos todas
			 */
			for(i=0;i<g_sControl_OS.cantidad_Tareas[prioridad];i++)  {
				tarea = (task *) g_sControl_OS.ListaTareas[prioridad][i];
				if(tarea->estado == TAREA_BLOCKED && tarea->ticks_bloqueada > 0)  {
					tarea->ticks_bloqueada--;

					/*
					 * Hacemos un checkeo de si la tarea apuntada con delay con bloqueo ya esta lista para
					 * ser tenida en cuenta. si es asi, la pasamos a Ready
					 */
					if (tarea->ticks_bloqueada == 0)
						tarea->estado = TAREA_READY;
				}
			}
		}
	}
	cpu_yield();			//en resumidas cuentas, llamamos a PendSV
}

/********************************************************************************
 * cpu_yield() llama a PendSV
 *******************************************************************************/
void cpu_yield(void)  {
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



/********************************************************************************
 * Enter critical deshabilita las interrupciones e incrementa el contador de las
 * veces que hemos llamado a la misma
 *******************************************************************************/
inline void os_enter_critical()  {
	__disable_irq();
	g_sControl_OS.contador_critico++;
}


/********************************************************************************
 * exit critical habilita las interrupciones si el contador global de las
 * solicitudes de secciones criticas llego a cero
 *******************************************************************************/
inline void os_exit_critical()  {
	if (--g_sControl_OS.contador_critico <= 0)  {
		g_sControl_OS.contador_critico = 0;
		__enable_irq();
	}
}
