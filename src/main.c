/*==================[inclusions]=============================================*/

#include <stdint.h>
#include <stdlib.h>
#include "main.h"

#include "g_OS_Core.h"
#include "g_OS_API.h"
#include "board.h"
#include "sapi.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/

task g_sTarea1, g_sTarea2, g_sTarea3, g_sBotones;
osSemaforo semLed1, semLed2, semLed3;

/*==================[external data definition]===============================*/



/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
}

/*==================[external functions definition]==========================*/




void task1(void)  {
	int i = 0;

	while (1) {
		i++;

		if (i%9 == 0)
			os_Semaforo_take(&semLed1);
		gpioToggle(LED1);
		os_Delay((rand() % 700));
	}
}

void task2(void)  {
	int j = 0;

	while (1) {
		j++;

		if (j%9 == 0)
			os_Semaforo_take(&semLed2);
		gpioToggle(LED2);
		os_Delay((rand() % 1300));
	}
}

void task3(void)  {
	int k = 0;

	while (1) {
		k++;
		os_Semaforo_take(&semLed3);
		gpioToggle(LED3);
		os_Delay(1);
		gpioToggle(LED3);
		os_Delay(1000);
	}
}

void botones(void)  {
	while(1)  {
		if(!gpioRead( TEC1 ))
			os_Semaforo_give(&semLed1);

		if(!gpioRead( TEC2 ))
			os_Semaforo_give(&semLed2);

		if(!gpioRead( TEC3 ))
			os_Semaforo_give(&semLed3);

		os_Delay(100);
	}
}




int main(void)  {
	initHardware();

	os_init_task(task1, &g_sTarea1,PRIORIDAD_2);
	os_init_task(task2, &g_sTarea2,PRIORIDAD_2);
	os_init_task(task3, &g_sTarea3,PRIORIDAD_2);
	os_init_task(botones,&g_sBotones,PRIORIDAD_7);

	os_Semaforo_init(&semLed1);
	os_Semaforo_init(&semLed2);
	os_Semaforo_init(&semLed3);

	os_start();

	while (1) {
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
