/*
 * spi0.h
 *
 *  Created on: 24-Jul-2020
 *      Author: AMIT VARMA
 */

#ifndef SPI0_H_
#define SPI0_H_
#include "gpio.h"
#define TX_MASK 32 //MOSI

#define FSS_MASK 8
#define CLK_MASK 4

#define SSI0RX PORTA,4

#define CS (*((volatile uint32_t *)(0x42000000 + (0x400043FC - 0x40000000)*32 + 3*4)))

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tm4c123gh6pm.h"

void initSpi0();
void writeSpi0(uint32_t data);
void EnableCS();
void DisableCS();
uint32_t readSpi0();

unsigned char spi1_send_read_byte(unsigned char byte);

#endif /* SPI0_H_ */
