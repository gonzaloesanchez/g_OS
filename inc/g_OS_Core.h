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

extern uint32_t pila1[STACK_SIZE/4];
extern uint32_t pila2[STACK_SIZE/4];

extern uint32_t sp1;
extern uint32_t sp2;


struct _task  {
	uint32_t pila[STACK_SIZE/4];
	uint32_t sp;
	void *entry_point;
	uint8_t task_id;
	uint8_t prioridad;
	char nombre[TASK_NAME_SIZE];
	struct _task *tarea_siguiente;
};

typedef struct _task task;


void init_task(void *tarea, task *tarea_struct, uint8_t prioridad);
void start_os(task *primera_tarea);
void ReturnHook(void);


#endif /* SIST_OPERATIVOS1_G_OS_INC_STUBS_H_ */
