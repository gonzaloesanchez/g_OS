/*
 * g_OS_IRQ.c
 *
 *  Created on: 29 abr. 2018
 *      Author: gonza
 */


#include "g_OS_IRQ.h"


static void* irq_vector_usuario[CANT_IRQ];				//vector de punteros a funciones para nuestras interrupciones


/********************************************************************************
 * Install interrupt. Debemos pasarle el tipo de interrupcion y la funcion del
 * usuario que desea instalar para atender esa interrupcion.
 * La funcion devuelve TRUE si fue exitosa o FALSE en caso contrario
 *******************************************************************************/
bool os_install_IRQ(LPC43XX_IRQn_Type irq, void* usr_irq)  {
	bool Ret = 0;

	/*
	 * Determinamos si la insterrupcion no fue definida anteriormente por el usuario
	 * Entonces cargamos el puntero de la funcion del usuario y  habilitamos esa interrupcion
	 * en el NVIC
	 */

	if (irq_vector_usuario[irq] == NULL) {
		irq_vector_usuario[irq] = usr_irq;
		NVIC_ClearPendingIRQ(irq);
		NVIC_EnableIRQ(irq);
		Ret = true;
	}


	return Ret;
}

/********************************************************************************
 * Remove interrupt. Debemos pasarle el tipo de interrupcion que queremos quitar
 * del vector que definimos con los punteros a funciones del usuario.
 * La funcion devuelve TRUE si fue exitosa o FALSE en caso contrario
 *******************************************************************************/
bool os_remove_IRQ(LPC43XX_IRQn_Type irq)  {
	bool Ret = 0;

	if (irq_vector_usuario[irq] != NULL) {
		irq_vector_usuario[irq] = NULL;
		NVIC_ClearPendingIRQ(irq);
		NVIC_DisableIRQ(irq);
		Ret = true;
	}

	return Ret;
}



/********************************************************************************
 * Esta funcion es la que todas las interrupciones llaman. Se encarga de llamar
 * a la funcion de usuario que haya sido cargada. LAS FUNCIONES DE USUARIO
 * LLAMADAS POR ESTA FUNCION SE EJECUTAN EN MODO HANDLER DE IGUAL FORMA. CUIDADO
 * CON LA CARGA DE CODIGO EN ELLAS, MISMAS REGLAS QUE EN BARE METAL
 *******************************************************************************/
static void os_irq_handler(LPC43XX_IRQn_Type IRQn)  {
	estadoOS estadoPrevio_OS;
	void (*funcion_usuario)(void);

	/*
	 * Guardamos el estado del sistema para restablecerlo al salir de
	 * la interrupcion. Este estado nos permite utilizar la misma api
	 * de sistema operativo para todos los casos
	 */
	estadoPrevio_OS = g_sControl_OS.estado_sistema;

	/*
	 * Actualizamos el estado del sistema operativo
	 */

	g_sControl_OS.estado_sistema = OS_IRQ;

	/*
	 * Llamamos a la funcion definida por el usuario
	 */
	funcion_usuario = irq_vector_usuario[IRQn];
	funcion_usuario();

	/*
	 * Retomamos el estado anterior de sistema operativo
	 */
	g_sControl_OS.estado_sistema = estadoPrevio_OS;

	/*
	 * Si hubo alguna llamada desde una interrupcion a una api liberando un evento, entonces
	 * llamamos al scheduler
	 */
	if (g_sControl_OS.llamar_scheduler)  {
		g_sControl_OS.llamar_scheduler = false;
		cpu_yield();
	}
}

/*==================[interrupt service routines]=============================*/

void DAC_IRQHandler(void){os_irq_handler(         DAC_IRQn         );}
void M0APP_IRQHandler(void){os_irq_handler(       M0APP_IRQn       );}
void DMA_IRQHandler(void){os_irq_handler(         DMA_IRQn         );}
void FLASH_EEPROM_IRQHandler(void){os_irq_handler(RESERVED1_IRQn   );}
void ETH_IRQHandler(void){os_irq_handler(         ETHERNET_IRQn    );}
void SDIO_IRQHandler(void){os_irq_handler(        SDIO_IRQn        );}
void LCD_IRQHandler(void){os_irq_handler(         LCD_IRQn         );}
void USB0_IRQHandler(void){os_irq_handler(        USB0_IRQn        );}
void USB1_IRQHandler(void){os_irq_handler(        USB1_IRQn        );}
void SCT_IRQHandler(void){os_irq_handler(         SCT_IRQn         );}
void RIT_IRQHandler(void){os_irq_handler(         RITIMER_IRQn     );}
void TIMER0_IRQHandler(void){os_irq_handler(      TIMER0_IRQn      );}
void TIMER1_IRQHandler(void){os_irq_handler(      TIMER1_IRQn      );}
void TIMER2_IRQHandler(void){os_irq_handler(      TIMER2_IRQn      );}
void TIMER3_IRQHandler(void){os_irq_handler(      TIMER3_IRQn      );}
void MCPWM_IRQHandler(void){os_irq_handler(       MCPWM_IRQn       );}
void ADC0_IRQHandler(void){os_irq_handler(        ADC0_IRQn        );}
void I2C0_IRQHandler(void){os_irq_handler(        I2C0_IRQn        );}
void SPI_IRQHandler(void){os_irq_handler(         I2C1_IRQn        );}
void I2C1_IRQHandler(void){os_irq_handler(        SPI_INT_IRQn     );}
void ADC1_IRQHandler(void){os_irq_handler(        ADC1_IRQn        );}
void SSP0_IRQHandler(void){os_irq_handler(        SSP0_IRQn        );}
void SSP1_IRQHandler(void){os_irq_handler(        SSP1_IRQn        );}
void UART0_IRQHandler(void){os_irq_handler(       USART0_IRQn      );}
void UART1_IRQHandler(void){os_irq_handler(       UART1_IRQn       );}
void UART2_IRQHandler(void){os_irq_handler(       USART2_IRQn      );}
void UART3_IRQHandler(void){os_irq_handler(       USART3_IRQn      );}
void I2S0_IRQHandler(void){os_irq_handler(        I2S0_IRQn        );}
void I2S1_IRQHandler(void){os_irq_handler(        I2S1_IRQn        );}
void SPIFI_IRQHandler(void){os_irq_handler(       RESERVED4_IRQn   );}
void SGPIO_IRQHandler(void){os_irq_handler(       SGPIO_INT_IRQn   );}
void GPIO0_IRQHandler(void){os_irq_handler(       PIN_INT0_IRQn    );}
void GPIO1_IRQHandler(void){os_irq_handler(       PIN_INT1_IRQn    );}
void GPIO2_IRQHandler(void){os_irq_handler(       PIN_INT2_IRQn    );}
void GPIO3_IRQHandler(void){os_irq_handler(       PIN_INT3_IRQn    );}
void GPIO4_IRQHandler(void){os_irq_handler(       PIN_INT4_IRQn    );}
void GPIO5_IRQHandler(void){os_irq_handler(       PIN_INT5_IRQn    );}
void GPIO6_IRQHandler(void){os_irq_handler(       PIN_INT6_IRQn    );}
void GPIO7_IRQHandler(void){os_irq_handler(       PIN_INT7_IRQn    );}
void GINT0_IRQHandler(void){os_irq_handler(       GINT0_IRQn       );}
void GINT1_IRQHandler(void){os_irq_handler(       GINT1_IRQn       );}
void EVRT_IRQHandler(void){os_irq_handler(        EVENTROUTER_IRQn );}
void CAN1_IRQHandler(void){os_irq_handler(        C_CAN1_IRQn      );}
void ADCHS_IRQHandler(void){os_irq_handler(       ADCHS_IRQn       );}
void ATIMER_IRQHandler(void){os_irq_handler(      ATIMER_IRQn      );}
void RTC_IRQHandler(void){os_irq_handler(         RTC_IRQn         );}
void WDT_IRQHandler(void){os_irq_handler(         WWDT_IRQn        );}
void M0SUB_IRQHandler(void){os_irq_handler(       M0SUB_IRQn       );}
void CAN0_IRQHandler(void){os_irq_handler(        C_CAN0_IRQn      );}
void QEI_IRQHandler(void){os_irq_handler(         QEI_IRQn         );}



//============================= Systick Handler =================================
void SysTick_Handler(void)  {
	uint8_t prioridad,i;
	task *tarea;

	/*
	 * Systick justamente es el que se encarga de ver todos los temporizadores
	 * por lo que recorremos todas las tareas que esten definidas y si tienen un
	 * valor de ticks de bloqueo mayor a cero, lo decrementamos.
	 *
	 */
	for (prioridad=0;prioridad<CANT_PRIORIDADES;prioridad++)  {
		if (g_sControl_OS.cantidad_Tareas[prioridad] > 0)  {
			/*
			 * Si existen tareas en esta lista de prioridad, las recorremos todas
			 */
			for(i=0;i<g_sControl_OS.cantidad_Tareas[prioridad];i++)  {
				tarea = (task *) g_sControl_OS.ListaTareas[prioridad][i];
				if(tarea->estado == TAREA_BLOCKED && tarea->ticks_bloqueada > 0)  {
					tarea->ticks_bloqueada--;

					/*
					 * Hacemos un checkeo de si la tarea apuntada con delay con bloqueo ya esta lista para
					 * ser tenida en cuenta. si es asi, la pasamos a Ready
					 */
					if (tarea->ticks_bloqueada == 0)
						tarea->estado = TAREA_READY;
				}
			}
		}
	}

	/*
	 * La implementacion de tickhook puede ser hecha por el usuario
	 */
	TickHook();
	cpu_yield();			//en resumidas cuentas, llamamos a PendSV
}
