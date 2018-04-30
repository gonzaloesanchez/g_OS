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

uint32_t contador_ticks = 0;		//contador de ticks global definido por el usuario

uint32_t medicion1 = 0;				//tiempo de la medicion 1
uint32_t medicion2 = 0;				//tiempo de la medicion 2

enum _Botones {T1_1_T2_1,
				T1_0_T2_1_FW,
				T1_0_T2_1_BK,
				T1_1_T2_0_FW,
				T1_1_T2_0_BK,
				T1_0_T2_0};			//estados de nuestra maquina de estados

typedef enum _Botones eBotones;

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

task g_sTarea_Led1_AB,g_sTarea_Led1_BA;
task g_sTarea_Led2_AB,g_sTarea_Led2_BA;
task g_sTarea_ME;

osSemaforo sem_Medicion1_AB,sem_Medicion1_BA;
osSemaforo sem_Medicion2_CD, sem_Medicion2_DC;
osSemaforo sem_ActualizarME;

bool g_bTecla1 = 0;
bool g_bTecla2 = 0;

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


/*****************************************************************************
 * 							SECCION DE TAREAS
 ****************************************************************************/

void Led1_AB(void)  {
	while (1) {

		/*
		 * Debemos esperar al evento en que la medicion es correcta
		 */
		os_Semaforo_take(&sem_Medicion1_AB);

		/*
		 * Una vez que ocurre la medicion correcta, tomamos el valor del contador de
		 * ticks correspondiente. Este caso corresponde a {0:1:ticks}\r\n
		 */

		gpioWrite(LEDG,true);
		gpioWrite(LED1,true);
		os_Delay(medicion1);
		gpioWrite(LED1,false);
		gpioWrite(LEDG,false);
	}
}

void Led1_BA(void)  {
	while (1) {

		/*
		 * Debemos esperar al evento en que la medicion es correcta
		 */
		os_Semaforo_take(&sem_Medicion1_BA);

		/*
		 * Una vez que ocurre la medicion correcta, tomamos el valor del contador de
		 * ticks correspondiente. Este caso corresponde a {0:1:ticks}\r\n
		 */

		gpioWrite(LEDB,true);
		gpioWrite(LED1,true);
		os_Delay(medicion1);
		gpioWrite(LED1,false);
		gpioWrite(LEDB,false);
	}
}


void Led2_AB(void)  {

	while (1) {
		/*
		 * Debemos esperar al evento en que la tecla se presiona
		 */
		os_Semaforo_take(&sem_Medicion2_CD);

		/*
		 * Una vez que ocurre la medicion correcta, tomamos el valor del contador de
		 * ticks correspondiente. Este caso corresponde a {0:1:ticks}\r\n
		 */

		gpioWrite(LEDG,true);
		gpioWrite(LED2,true);
		os_Delay(medicion2);
		gpioWrite(LED2,false);
		gpioWrite(LEDG,false);
	}
}

void Led2_BA(void)  {

	while (1) {
		/*
		 * Debemos esperar al evento en que la tecla se presiona
		 */
		os_Semaforo_take(&sem_Medicion2_DC);

		/*
		 * Una vez que ocurre la medicion correcta, tomamos el valor del contador de
		 * ticks correspondiente. Este caso corresponde a {0:1:ticks}\r\n
		 */

		gpioWrite(LEDB,true);
		gpioWrite(LED2,true);
		os_Delay(medicion2);
		gpioWrite(LED2,false);
		gpioWrite(LEDB,false);
	}
}


void MaquinaEstados(void)  {
	eBotones MaquinaBotones = T1_1_T2_1;		//inicializo maquina de estados

	while(1)  {
		os_Semaforo_take(&sem_ActualizarME);

		switch (MaquinaBotones)  {

		/*
		 * Ambas teclas sin presionar. Solo podemos ir a los estados forward (FW)
		 */
		case T1_1_T2_1:
			if (g_bTecla1 == false)  {
				MaquinaBotones = T1_0_T2_1_FW;
				contador_ticks = 0;
			}
			else if (g_bTecla2 == false)  {
				MaquinaBotones = T1_1_T2_0_FW;
				contador_ticks = 0;
			}
			break;

		/*
		 * En este estado se pueden dar dos opciones:
		 *
		 * 1) Se presiono y luego solto la tecla 1 sin presionar la tecla 2
		 *    ____       ___________   (1)
		 * T1     |______|			   (0)
		 *
		 * 	  ______________________   (1)
		 * T2						   (0)
		 *
		 *
		 * 2) Se presiono la tecla 2 luego de presionar la tecla 1
		 *    ____       			   (1)
		 * T1     |____________		   (0)
		 *
		 * 	  ___________   		   (1)
		 * T2			 |_______	   (0)
		 */
		case T1_0_T2_1_FW:
			if (g_bTecla1 == true)  {
				MaquinaBotones = T1_1_T2_1;		//no pasa nada, volvemos a esperar
			}
			else if (g_bTecla2 == false)  {
				MaquinaBotones = T1_0_T2_0;
				/*
				 * Este caso es la medicion AB (tiempo entre flancos B - A segun el enunciado)
				 */
				medicion1 = contador_ticks;
				os_Semaforo_give(&sem_Medicion1_AB);
			}
			break;

		/*
		 * En este estado se pueden dar dos opciones:
		 *
		 * 1) Se presiono y luego solto la tecla 2 sin presionar la tecla 1
		 *    ______________________   (1)
		 * T1     					   (0)
		 *
		 * 	  ___        ____________   (1)
		 * T2    |______|			    (0)
		 *
		 *
		 * 2) Se presiono la tecla 1 luego de presionar la tecla 2
		 *    ___________    			(1)
		 * T1            |_______		(0)
		 *
		 * 	  _____   		   			(1)
		 * T2      |____________  		(0)
		 */
		case T1_1_T2_0_FW:
			if (g_bTecla2 == true)  {
				MaquinaBotones = T1_1_T2_1;		//no pasa nada, volvemos a esperar
			}
			else if (g_bTecla1 == false)  {
				MaquinaBotones = T1_0_T2_0;
				/*
				 * Este caso es la medicion BA (tiempo entre flancos A - B segun el enunciado)
				 */
				medicion1 = contador_ticks;
				os_Semaforo_give(&sem_Medicion1_BA);
			}
			break;

		/*
		 * Ambas teclas presionadas
		 */
		case T1_0_T2_0:
			if (g_bTecla2 == true)  {
				MaquinaBotones = T1_0_T2_1_BK;		//pasamos al estado correspondiente
				contador_ticks = 0;
			}
			else if (g_bTecla1 == true)  {
				MaquinaBotones = T1_1_T2_0_BK;		//pasamos al estado correspondiente
				contador_ticks = 0;
			}

			break;


		/*
		 * En este estado se pueden dar dos opciones:
		 *
		 * 1) Se solto y luego volvio a presionar la tecla 2 sin soltar la tecla 1
		 *
		 *     				 		    (1)
		 * T1 ______________________    (0)
		 *
		 * 	       ____   			    (1)
		 * T2 ____|	   |____________    (0)
		 *
		 *
		 * 2) Se solto la tecla 1 luego de soltar la tecla 2
		 *        		   _________ 	(1)
		 * T1  ___________|        		(0)
		 *
		 * 	       _________________	(1)
		 * T2  ___|  					(0)
		 */
		case T1_0_T2_1_BK:
			if (g_bTecla2 == false)  {
				MaquinaBotones = T1_0_T2_0;		//no pasa nada, volvemos a esperar
			}
			else if (g_bTecla1 == true)  {
				MaquinaBotones = T1_1_T2_1;
				/*
				 * Este caso es la medicion AB (tiempo entre flancos B - A segun el enunciado)
				 */
				medicion2 = contador_ticks;
				os_Semaforo_give(&sem_Medicion2_CD);
			}
			break;


		/*
		 * En este estado se pueden dar dos opciones:
		 *
		 * 1) Se solto y luego volvio a presionar la tecla 1 sin soltar la tecla 2
		 *
		 *     	   ____		 		    (1)
		 * T1 ____|	   |____________    (0)
		 *
		 * 	       		  			    (1)
		 * T2 ______________________    (0)
		 *
		 *
		 * 2) Se solto la tecla 2 luego de soltar la tecla 1
		 *
		 *           _______________ 	(1)
		 * T1  _____|        			(0)
		 *
		 * 	                ________	(1)
		 * T2  ____________|  			(0)

		 */
		case T1_1_T2_0_BK:
			if (g_bTecla1 == false)  {
				MaquinaBotones = T1_0_T2_0;		//no pasa nada, volvemos a esperar
			}
			else if (g_bTecla2 == true)  {
				MaquinaBotones = T1_1_T2_1;
				/*
				 * Este caso es la medicion AB (tiempo entre flancos B - A segun el enunciado)
				 */
				medicion2 = contador_ticks;
				os_Semaforo_give(&sem_Medicion2_DC);
			}

			break;

		default:
			;
		}
	}



}
//================================================================================

int main(void)  {
	initHardware();

	os_init_task(Led1_AB, &g_sTarea_Led1_AB,PRIORIDAD_1);
	os_init_task(Led1_BA, &g_sTarea_Led1_BA,PRIORIDAD_1);
	os_init_task(Led2_AB, &g_sTarea_Led2_AB,PRIORIDAD_1);
	os_init_task(Led2_BA, &g_sTarea_Led2_BA,PRIORIDAD_1);
	os_init_task(MaquinaEstados, &g_sTarea_ME,PRIORIDAD_0);

	os_Semaforo_init(&sem_Medicion1_AB);
	os_Semaforo_init(&sem_Medicion1_BA);
	os_Semaforo_init(&sem_Medicion2_CD);
	os_Semaforo_init(&sem_Medicion2_DC);
	os_Semaforo_init(&sem_ActualizarME);

	os_install_IRQ(PIN_INT0_IRQn,tecla1_down_IRQ);
	os_install_IRQ(PIN_INT1_IRQn,tecla1_up_IRQ);
	os_install_IRQ(PIN_INT2_IRQn,tecla2_down_IRQ);
	os_install_IRQ(PIN_INT3_IRQn,tecla2_up_IRQ);

	os_start();

	while (1) {
	}
}



void TickHook(void)  {
	contador_ticks++;
}



void tecla1_down_IRQ(void) {
	os_Semaforo_give(&sem_ActualizarME);
	g_bTecla1 = false;

	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 0 ) );
}

void tecla1_up_IRQ(void)  {
	os_Semaforo_give(&sem_ActualizarME);
	g_bTecla1 = true;

	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 1 ) );
}

void tecla2_down_IRQ(void) {
	os_Semaforo_give(&sem_ActualizarME);
	g_bTecla2 = false;

	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 2 ) );
}

void tecla2_up_IRQ(void)  {
	os_Semaforo_give(&sem_ActualizarME);
	g_bTecla2 = true;

	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 3 ) );
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
