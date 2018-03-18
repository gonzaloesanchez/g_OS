/*
 * Leds.h
 *
 *  Created on: 7/9/2017
 *      Author: Gonza
 */

#ifndef LEDS_H_
#define LEDS_H_

#include <stdbool.h>
#include <stdint.h>
#include "chip.h"

/**
 * Definciones de hardware
 */
#define LEDR_P    2
#define LEDR_P_   0
#define LEDR_GPIO 5
#define LEDR_PIN  0

#define LEDG_P    2
#define LEDG_P_   1
#define LEDG_GPIO 5
#define LEDG_PIN  1

#define LEDB_P    2
#define LEDB_P_   2
#define LEDB_GPIO 5
#define LEDB_PIN  2

#define LED1_P    2
#define LED1_P_  10
#define LED1_GPIO 0
#define LED1_PIN 14

#define LED2_P    2
#define LED2_P_  11
#define LED2_GPIO 1
#define LED2_PIN 11

#define LED3_P    2
#define LED3_P_  12
#define LED3_GPIO 1
#define LED3_PIN 12

/******************************************************************************
 * Definciones del modulo
 *****************************************************************************/

#define  LED_VERDE_RGB  0
#define  LED_ROJO_RGB   1
#define  LED_AZUL_RGB   2
#define  LED_ROJO_1     3
#define  LED_AMARILLO_2 4
#define  LED_VERDE_3    5

/******************************************************************************
 * Prototipo de funciones
 *****************************************************************************/

void led_set(uint8_t led_nro, bool X);
void led_toogle(uint8_t led_nro);


#endif /* LEDS_H_ */
