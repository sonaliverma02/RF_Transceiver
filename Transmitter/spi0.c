/*
 * spi0.c
 *
 *  Created on: 24-Jul-2020
 *      Author: AMIT VARMA
 */
#include "spi0.h"

void initSpi0()
{
    // Enable GPIO port A peripherals
          SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
    // Enable clocks
      SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0;


      GPIO_PORTA_DIR_R |= TX_MASK | FSS_MASK | CLK_MASK; // make SSI1 TX, FSS, and CLK outputs
      GPIO_PORTA_DR2R_R |= TX_MASK | FSS_MASK | CLK_MASK; // set drive strength to 2mA
      GPIO_PORTA_DEN_R |= TX_MASK | FSS_MASK | CLK_MASK; // enable digital operation
      GPIO_PORTA_AFSEL_R |= TX_MASK |  CLK_MASK; // select alternative functions
            GPIO_PORTA_PCTL_R = GPIO_PCTL_PA5_SSI0TX | GPIO_PCTL_PA3_SSI0FSS | GPIO_PCTL_PA2_SSI0CLK; // map alt fns to SSI1

          // GPIO_PORTA_PUR_R |= CLK_MASK;

            selectPinDigitalInput(SSI0RX);
                  setPinAuxFunction(SSI0RX, GPIO_PCTL_PA4_SSI0RX);

      // Configure SSI0 pins for SPI configuration
            SSI0_CR1_R &= ~SSI_CR1_SSE;
            SSI0_CR1_R = 0;                                    // select master mode
                  SSI0_CC_R = 0;                                     // select system clock as the clock source
                  SSI0_CPSR_R = (40/4);                                  // set bit rate to 4 MHz (if SR=0 in CR0)
                  SSI0_CR0_R =  SSI_CR0_FRF_MOTO | SSI_CR0_DSS_8; // set SR=0, mode 3 (SPH=1, SPO=1), 16-bit
                  SSI0_CR1_R |= SSI_CR1_SSE;                         // turn on SSI1
}

void writeSpi0(uint32_t data)
{
    SSI0_DR_R  = data ;
    while(SSI0_SR_R & SSI_SR_BSY);
}

uint32_t readSpi0()
{
    return  SSI0_DR_R;
}

void EnableCS()
{
    CS = 0;
}

void DisableCS()
{
    CS = 1;
}

unsigned char spi1_send_read_byte(unsigned char byte)
{
    SSI0_DR_R = byte;
    while (SSI0_SR_R & SSI_SR_BSY);

    return SSI0_DR_R ;
}
