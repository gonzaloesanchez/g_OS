/*
 * stubs.h
 *
 *  Created on: 10 mar. 2018
 *      Author: gonza
 */


#ifndef SIST_OPERATIVOS1_G_OS_INC_STUBS_H_
#define SIST_OPERATIVOS1_G_OS_INC_STUBS_H_

#include <stdint.h>

#define STACK_SIZE 512


void init_task(void *task, uint32_t *stack, uint32_t *stack_pointer);
void start_os(void);


#endif /* SIST_OPERATIVOS1_G_OS_INC_STUBS_H_ */
