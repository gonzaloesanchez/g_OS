/*
 * Leds.c
 *
 *  Created on: 7/9/2017
 *      Author: Gonza
 */


#include "Leds.h"

static bool estadoLeds[6];

void led_set(uint8_t led_nro, bool X)  {

   estadoLeds[led_nro] = X;

   switch (led_nro)  {
   case LED_VERDE_RGB:
      Chip_GPIO_SetPinState( LPC_GPIO_PORT, LEDG_GPIO, LEDG_PIN, X);
      break;

   case LED_ROJO_RGB:
      Chip_GPIO_SetPinState( LPC_GPIO_PORT, LEDR_GPIO, LEDR_PIN, X);
      break;

   case LED_AZUL_RGB:
      Chip_GPIO_SetPinState( LPC_GPIO_PORT, LEDB_GPIO, LEDB_PIN, X);
      break;

   case LED_ROJO_1:
      Chip_GPIO_SetPinState( LPC_GPIO_PORT, LED1_GPIO, LED1_PIN, X);
      break;

   case LED_AMARILLO_2:
      Chip_GPIO_SetPinState( LPC_GPIO_PORT, LED2_GPIO, LED2_PIN, X);
      break;

   case LED_VERDE_3:
      Chip_GPIO_SetPinState( LPC_GPIO_PORT, LED3_GPIO, LED3_PIN, X);
      break;

   default:;
   }
}

void led_toogle(uint8_t led_nro)  {
   estadoLeds[led_nro] = !estadoLeds[led_nro];

   switch (led_nro)  {
   case LED_VERDE_RGB:
      Chip_GPIO_SetPinState( LPC_GPIO_PORT, LEDG_GPIO, LEDG_PIN, estadoLeds[led_nro]);
      break;

   case LED_ROJO_RGB:
      Chip_GPIO_SetPinState( LPC_GPIO_PORT, LEDR_GPIO, LEDR_PIN, estadoLeds[led_nro]);
      break;

   case LED_AZUL_RGB:
      Chip_GPIO_SetPinState( LPC_GPIO_PORT, LEDB_GPIO, LEDB_PIN, estadoLeds[led_nro]);
      break;

   case LED_ROJO_1:
      Chip_GPIO_SetPinState( LPC_GPIO_PORT, LED1_GPIO, LED1_PIN, estadoLeds[led_nro]);
      break;

   case LED_AMARILLO_2:
      Chip_GPIO_SetPinState( LPC_GPIO_PORT, LED2_GPIO, LED2_PIN, estadoLeds[led_nro]);
      break;

   case LED_VERDE_3:
      Chip_GPIO_SetPinState( LPC_GPIO_PORT, LED3_GPIO, LED3_PIN, estadoLeds[led_nro]);
      break;

   default:;
   }
}
