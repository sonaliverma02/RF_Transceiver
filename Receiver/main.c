/*
 * spiconfig.c
 *
 *  Created on: 22-Jul-2020
 *      Author: Sonali Verma
 *      //Configuration
    //GPIO_PA2_SSI0CLK
        //GPIO_PA4_SSI0RX
        //GPIO_PA5_SSI0TX
         // CE PC4
          // CSN PA3
           * IRQ PB0
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tm4c123gh6pm.h"

#include "spi0.h"
#include "uart0.h"
#include "wait.h"
#include "nrf24l01.h"


// Pins
#define BLUE_LED     (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4))) // on-board blue LED
#define  BLUE_LED_MASK 4
char* spidata = "I'm from TM4C123GH6PM";

void initHw1()
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
    // Configure Blue LED (on-board) ON PF2
            GPIO_PORTF_DIR_R |= BLUE_LED_MASK;  // bit 2 is output, other pins are inputs
            GPIO_PORTF_DR2R_R |= BLUE_LED_MASK; // set drive strength to 2mA (not needed since default configuration -- for clarity)
            GPIO_PORTF_DEN_R |= BLUE_LED_MASK;  // enable LED
   GPIO_PORTC_DIR_R |= CE_MASK;  // bit 2 is output, other pins are inputs
          GPIO_PORTC_DR2R_R |= CE_MASK; // set drive strength to 2mA (not needed since default configuration -- for clarity)
          GPIO_PORTC_DEN_R |= CE_MASK;
          //GPIO_PORTC_PCTL_R |= CE_MASK;
          //GPIO_PORTC_PUR_R |= CE_MASK;
         // CE =1;

}
void ToggleLED()
{
    BLUE_LED ^=1;
}

int main(void)
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
         //SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

         // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
         // Note UART on port A must use APB
     //    SYSCTL_GPIOHBCTL_R = 0;

    // Enable GPIO port A C D F E peripherals
        /*            SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;

                    // Configure Red Orange Yellow LEDs (off-board) PA2 PA3 PA4
                        GPIO_PORTA_DIR_R |= FSS_MASK;  // bits 2,3,4 are output, other pins are inputs
                        GPIO_PORTA_DR2R_R |= FSS_MASK;  // set drive strength to 2mA (not needed since default configuration -- for clarity)
                        GPIO_PORTA_DEN_R |= FSS_MASK;  // enable LEDs

                        CS = 0;

                        CS = 1;*/

    initHw1();
    initSpi0();
    // Setup UART0
          initUart0();
          setUart0BaudRate(115200, 40e6);
          nrf24l01_initialize_debug(true, 3, false); //initialize the 24L01 to the debug configuration as TX,
    //1 data byte, and auto-ack disabled
    unsigned char* data; //register to hold letter sent and received
    unsigned int count; //counter for for loop
    while(1)
     {
        //wait until a packet has been received
        while(!(nrf24l01_irq_pin_active() ));//&& nrf24l01_irq_rx_dr_active()

        nrf24l01_read_rx_payload(data, 3); //read the packet into data
        putsUart0((char*)data);
        nrf24l01_irq_clear_all(); //clear all interrupts in the 24L01

       waitMicrosecond(130); //wait for the other 24L01 to come from standby to RX

        nrf24l01_set_as_tx(); //change the device to a TX to send back from the other 24L01
        nrf24l01_write_tx_payload(data, 3, true); //transmit received char over RF

        //wait until the packet has been sent
        while(!(nrf24l01_irq_pin_active() && nrf24l01_irq_tx_ds_active()));

        nrf24l01_irq_clear_all(); //clear interrupts again
        nrf24l01_set_as_rx(true); //resume normal operation as an RX

        ToggleLED(); //toggle the on-board LED as visual indication that the loop has completed
     }

    /*while(1)
    {
   // int n = strlen(spidata);
        if(*spidata==0){
            DisableCS();
            break;
        }


    EnableCS();

    writeSpi0(*spidata);
    waitMicrosecond(1e6);
    spidata++;
   // DisableCS();


    }*/
}


