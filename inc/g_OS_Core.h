/*
 * stubs.h
 *
 *  Created on: 10 mar. 2018
 *      Author: gonza
 */

/**
 * Para definir una tarea en el sistema operativo g_OS es necesario declarar una estructura task para cada tarea.
 * Esta estructura contendra la pila de la tarea, su stack pointer, prioridad y un nombre corto. Cada tarea se enlazara
 * con la siguiente y luego se determinara cual es la prioridad (funcion de prioridades a determinar)
 */

#ifndef SIST_OPERATIVOS1_G_OS_INC_STUBS_H_
#define SIST_OPERATIVOS1_G_OS_INC_STUBS_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "board.h"

#define STACK_SIZE 512


//definiciones para el OS
#define AUTO_STACKING_SIZE	8	// registros que hacen stacking automatico, sin contar Floating point registers
#define FULL_STACKING_SIZE	17	//Todos los regstros excepto FPunit
#define TASK_NAME_SIZE		10
#define MAX_TASK_NUM		8	//cantidad maxima de tareas a definir. Esto es 8 tareas por prioridad
								//En la prioridad mas baja tendremos una tarea menos, porque alli va IDLE

//definiciones de prioridades. Por ahora, no habra mas de 8 prioridades
#define CANT_PRIORIDADES	9
#define PRIORIDAD_0			0	//prioridad maxima, siguiendo la logica de la prioridad de interrupciones del core
#define PRIORIDAD_1			1
#define PRIORIDAD_2			2
#define PRIORIDAD_3			3
#define PRIORIDAD_4			4
#define PRIORIDAD_5			5
#define PRIORIDAD_6			6
#define PRIORIDAD_7			7	//prioridad minima
#define PRIORIDAD_IDLE		8	//esta prioridad no es visible al usuario, es donde va IDLE


//definiciones de codigos de error
#define ERR_OS_CANT_TAREAS		-1
#define ERR_PRIORITY_OVERFLOW	-100

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
#define LR_PREV_VALUE	9
#define R4	10
#define R5	11
#define R6	12
#define R7	13
#define R8	14
#define R9	15
#define R10 16
#define R11 17

//definiciones para el procesador
#define INIT_xPSR 	1 << 24
#define EXEC_RETURN	0xFFFFFFF9

#define __WEAK__   __attribute__((weak))

/********************************************************************************
 * Definicion de los estados posibles para las tareas
 *******************************************************************************/

enum _estadoTarea  {
	ESTADO_RESERVADO,
	TAREA_READY,
	TAREA_RUNNING,
	TAREA_BLOCKED,
	TAREA_HALTED
};

typedef enum _estadoTarea estadoTarea;


/********************************************************************************
 * Definicion de la estructura para cada tarea
 *******************************************************************************/
struct _task  {
	uint32_t pila[STACK_SIZE/4];
	uint32_t sp;
	void *entry_point;
	uint8_t task_id;
	estadoTarea estado;
	uint8_t prioridad;
	uint32_t ticks_bloqueada;
	char nombre[TASK_NAME_SIZE];
};

typedef struct _task task;

/********************************************************************************
 * Definicion de la estructura de control para el sistema operativo
 *******************************************************************************/
struct _osControl  {
	void *ListaTareas[CANT_PRIORIDADES][MAX_TASK_NUM];	//array de punteros a tareas
	int8_t Error;										//variable que contiene el ultimo error generado
	bool bStartOS;										//esta bandera es para el comienzo de cambio de contexto
	uint8_t cantidad_Tareas[CANT_PRIORIDADES];		//cantidad de tareas definidas por el usuario para cada prioridad
	int16_t contador_critico;

	task *spTarea_actual;				//definicion de puntero para tarea actual
	task *spTarea_siguiente;			//definicion de puntero para tarea siguiente
};
typedef struct _osControl osControl;


void os_init_task(void *tarea, task *tarea_struct, uint8_t prioridad);
void os_start(void);
void os_init_mem(void);
void cpu_yield(void);
void ReturnHook(void);


#endif /* SIST_OPERATIVOS1_G_OS_INC_STUBS_H_ */
