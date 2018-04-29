/*==================[inclusions]=============================================*/

#include <stdint.h>
#include <stdlib.h>
#include "main.h"

#include "g_OS_Core.h"
#include "g_OS_API.h"
#include "g_OS_IRQ.h"
#include "board.h"
#include "sapi.h"

/*==================[macros and definitions]=================================*/
#define TEC1_PORT_NUM   0
#define TEC1_BIT_VAL    4

#define TEC2_PORT_NUM   0
#define TEC2_BIT_VAL    8


#define LED1_PORT_NUM   0
#define LED1_BIT_VAL    14

#define LED2_PORT_NUM   1
#define LED2_BIT_VAL    11

/*==================[internal data declaration]==============================*/

uint32_t Contador_tecla1 = 0;		//contador de ticks global definido por el usuario
uint32_t Contador_tecla2 = 0;		//contador de ticks global definido por el usuario

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

void tecla1_down_IRQ(void);
void tecla1_up_IRQ(void);
void tecla2_down_IRQ(void);
void tecla2_up_IRQ(void);


/*==================[internal data definition]===============================*/

task g_sTarea_Tecla1, g_sTarea_Tecla2;
osSemaforo sem_Tecla1_Down, sem_Tecla1_Up, sem_Tecla2_Down, sem_Tecla2_Up;

/*==================[external data definition]===============================*/



/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);


	/*
	 * Seteamos la interrupcion 0 para el flanco descendente en la tecla 1
	 */
	Chip_SCU_GPIOIntPinSel( 0, TEC1_PORT_NUM, TEC1_BIT_VAL );
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 0 ) ); // INT0 flanco descendente
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( 0 ) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( 0 ) );

	/*
	 * Seteamos la interrupcion 1 para el flanco ascendente en la tecla 1
	 */
	Chip_SCU_GPIOIntPinSel( 1, TEC1_PORT_NUM, TEC1_BIT_VAL );
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 1 ) ); // INT1 flanc
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( 1 ) );
	Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH( 1 ) );

	/*
	 * Seteamos la interrupcion 2 para el flanco descendente en la tecla 2
	 */
	Chip_SCU_GPIOIntPinSel( 2, TEC2_PORT_NUM, TEC2_BIT_VAL );
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 2 ) ); // INT2
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( 2 ) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( 2 ) );

	/*
	 * Seteamos la interrupcion 3 para el flanco ascendente en la tecla 2
	 */
	Chip_SCU_GPIOIntPinSel( 3, TEC2_PORT_NUM, TEC2_BIT_VAL );
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 3 ) ); // INT3
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( 3 ) );
	Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH( 3 ) );
}

/*==================[external functions definition]==========================*/




void tecla1(void)  {
	uint32_t Contador_interno;
	while (1) {

		/*
		 * Debemos esperar al evento en que la tecla se presiona
		 */
		os_Semaforo_take(&sem_Tecla1_Down);

		/*
		 * Una vez presionada la tecla, reseteamos el contador te ticks. Este se
		 * incrementara en el tickhook (una vez por milisegundo)
		 */
		Contador_tecla1 = 0;

		/*
		 * Debemos entonces esperar a que se suelte la tecla
		 */
		os_Semaforo_take(&sem_Tecla1_Up);

		/*
		 * Una vez soltada la tecla, tomamos el valor del contador.
		 */
		Contador_interno = Contador_tecla1;

		gpioWrite(LED1,true);
		os_Delay(Contador_interno);
		gpioWrite(LED1,false);
	}
}


void tecla2(void)  {
	uint32_t Contador_interno;

	while (1) {
		/*
		 * Debemos esperar al evento en que la tecla se presiona
		 */
		os_Semaforo_take(&sem_Tecla2_Down);

		/*
		 * Una vez presionada la tecla, reseteamos el contador te ticks. Este se
		 * incrementara en el tickhook (una vez por milisegundo)
		 */
		Contador_tecla2 = 0;

		/*
		 * Debemos entonces esperar a que se suelte la tecla
		 */
		os_Semaforo_take(&sem_Tecla2_Up);

		/*
		 * Una vez soltada la tecla, tomamos el valor del contador.
		 */
		Contador_interno = Contador_tecla2;

		gpioWrite(LED2,true);
		os_Delay(Contador_interno);
		gpioWrite(LED2,false);
	}
}


int main(void)  {
	initHardware();

	os_init_task(tecla1, &g_sTarea_Tecla1,PRIORIDAD_0);
	os_init_task(tecla2, &g_sTarea_Tecla2,PRIORIDAD_0);

	os_Semaforo_init(&sem_Tecla1_Down);
	os_Semaforo_init(&sem_Tecla1_Up);
	os_Semaforo_init(&sem_Tecla2_Down);
	os_Semaforo_init(&sem_Tecla2_Up);

	os_install_IRQ(PIN_INT0_IRQn,tecla1_down_IRQ);
	os_install_IRQ(PIN_INT1_IRQn,tecla1_up_IRQ);
	os_install_IRQ(PIN_INT2_IRQn,tecla2_down_IRQ);
	os_install_IRQ(PIN_INT3_IRQn,tecla2_up_IRQ);

	os_start();

	while (1) {
	}
}



void TickHook(void)  {
	Contador_tecla1++;
	Contador_tecla2++;
}



void tecla1_down_IRQ(void) {
	os_Semaforo_give(&sem_Tecla1_Down);
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 0 ) );
}

void tecla1_up_IRQ(void)  {
	os_Semaforo_give(&sem_Tecla1_Up);
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 1 ) );
}

void tecla2_down_IRQ(void) {
	os_Semaforo_give(&sem_Tecla2_Down);
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 2 ) );
}

void tecla2_up_IRQ(void)  {
	os_Semaforo_give(&sem_Tecla2_Up);
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 3 ) );
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
