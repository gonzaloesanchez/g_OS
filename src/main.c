/*==================[inclusions]=============================================*/

#include <stdint.h>
#include "main.h"
#include "board.h"
#include "stubs.h"			//solamente para versiones prueba

/*==================[macros and definitions]=================================*/

#define STACK_SIZE 512

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/

static uint8_t pila1[STACK_SIZE];
static uint8_t pila2[STACK_SIZE];
static uint32_t sp1;
static uint32_t sp2;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
}

/*==================[external functions definition]==========================*/

void SysTick_Handler(void)
{
}

void task1(void)
{
	int i;
	while (1) {
		i++;
	}
}

void task2(void)
{
	int j;
	while (1) {
		j++;
	}
}

int main(void)
{
	initHardware();

	init_task(task1, pila1, sp1);
	init_task(task2, pila2, sp2);

	start_os();

	while (1) {
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
