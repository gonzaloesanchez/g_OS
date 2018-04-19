/*
 * g_OS_API.h
 *
 *  Created on: 10 abr. 2018
 *      Author: gonza
 */

#ifndef G_OS_INC_G_OS_API_H_
#define G_OS_INC_G_OS_API_H_


#include "g_OS_Core.h"

extern osControl g_sControl_OS;
extern uint32_t g_ui32Ticks;



/********************************************************************************
 * Definicion de la estructura para los semaforos
 *******************************************************************************/
struct _semaforo  {
	task* tarea_asociada;
	bool tomado;
};

typedef struct _semaforo osSemaforo;




//prototipos de funciones que el usuario puede poblar
__WEAK__ void ReturnHook(void);
__WEAK__ void taskIdle(void);
__WEAK__ void ErrorHook(void *Caller);

//prototipos de funciones de la API
void os_Delay(uint32_t ticks);
void os_Semaforo_init(osSemaforo* X);
void os_Semaforo_take(osSemaforo* X);
void os_Semaforo_give(osSemaforo* X);

#endif /* G_OS_INC_G_OS_API_H_ */
