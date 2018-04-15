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

task g_sTarea1, g_sTarea2, g_sTarea3;

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
		gpioToggle(LED1);
		os_Delay((rand() % 700));
	}
}

void task2(void)  {
	int j = 0;
	while (1) {
		j++;
		gpioToggle(LED2);
		//os_Delay((rand() % 1300));
	}
}

void task3(void)  {
	int k = 0;
	while (1) {
		k++;
		gpioToggle(LED3);
		//os_Delay(rand() % 300);
	}
}

int main(void)  {
	initHardware();

	os_init_mem();

	os_init_task(task1, &g_sTarea1,PRIORIDAD_2);
	os_init_task(task2, &g_sTarea2,PRIORIDAD_3);
	os_init_task(task3, &g_sTarea3,PRIORIDAD_3);

	os_start();

	while (1) {
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
