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
		osDelay((rand() % 700));
	}
}

void task2(void)  {
	int j = 0;
	while (1) {
		j++;
		gpioToggle(LED2);
		osDelay((rand() % 1300));
	}
}

void task3(void)  {
	int k = 0;
	while (1) {
		k++;
		gpioToggle(LED3);
		osDelay(rand() % 300);
	}
}

int main(void)  {
	initHardware();

	init_task(task1, &g_sTarea1,1);
	init_task(task2, &g_sTarea2,2);
	init_task(task3, &g_sTarea3,1);

	start_os();

	while (1) {
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
