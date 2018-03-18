/*==================[inclusions]=============================================*/

#include <stdint.h>
#include "main.h"

#include "g_OS_Core.h"
#include "board.h"
#include "Leds.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/

static uint32_t pila1[STACK_SIZE];
static uint32_t pila2[STACK_SIZE];

/*==================[external data definition]===============================*/

uint32_t sp1;
uint32_t sp2;


/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
}

/*==================[external functions definition]==========================*/

/*
void SysTick_Handler(void)
{
}
*/

/**
 * TODO:	Agregar Push & Pop de registros restantes
 */

void task1(void)  {
	while (1) {
		led_toogle(LED_ROJO_1);
	}
}

void task2(void)  {
	while (1) {
		led_toogle(LED_AMARILLO_2);
	}
}

int main(void)
{
	initHardware();

	init_task(task1, pila1, &sp1);
	init_task(task2, pila2, &sp2);

	start_os();

	while (1) {
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
