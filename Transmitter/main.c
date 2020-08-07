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
#define GREEN_LED_MASK 8
#define GREEN_LED     (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4))) // on-board GREEN LED
char* spidata = "I'm from TM4C123GH6PM";

void initHw1()
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC |SYSCTL_RCGC2_GPIOF;
   GPIO_PORTC_DIR_R |= CE_MASK;  // bit 2 is output, other pins are inputs
          GPIO_PORTC_DR2R_R |= CE_MASK; // set drive strength to 2mA (not needed since default configuration -- for clarity)
          GPIO_PORTC_DEN_R |= CE_MASK;
          // Configure Blue LED (on-board) ON PF2
                     GPIO_PORTF_DIR_R |= GREEN_LED_MASK;  // bit 2 is output, other pins are inputs
                     GPIO_PORTF_DR2R_R |= GREEN_LED_MASK; // set drive strength to 2mA (not needed since default configuration -- for clarity)
                     GPIO_PORTF_DEN_R |= GREEN_LED_MASK;  // enable LED
          //GPIO_PORTC_PCTL_R |= CE_MASK;
          //GPIO_PORTC_PUR_R |= CE_MASK;
         // CE =1;

}
void ToggleLED()
{
    GREEN_LED ^=1;
}
int main(void)
{


    initHw1();
    initSpi0();
    // Setup UART0
          initUart0();
          setUart0BaudRate(115200, 40e6);
          nrf24l01_initialize_debug(false, 3, false); //initialize the 24L01 to the debug configuration as TX,
    //1 data byte, and auto-ack disabled
    unsigned char* data; //register to hold letter sent and received
    unsigned int count; //counter for for loop
    while(1)
    {
        //while(kbhitUart0())

          // data = getcUart0();
        data = "hey";
           // spi1_send_read_byte(data);
             nrf24l01_write_tx_payload(data, strlen((const char*)data), true); //transmit received char over RF
            //wait until the packet has been sent or the maximum number of retries has been reached
        while(  !(  nrf24l01_irq_pin_active() && nrf24l01_irq_tx_ds_active()  )   );

            nrf24l01_irq_clear_all(); //clear all interrupts in the 24L01
            nrf24l01_set_as_rx(true); //change the device to an RX to get the character back from the other 24L01

            //wait a while to see if we get the data back (change the loop maximum and the lower if
            //  argument (should be loop maximum - 1) to lengthen or shorten this time frame
            for(count = 0; count < 20000; count++)
            {
                //check to see if the data has been received.  if so, get the data and exit the loop.
                //  if the loop is at its last count, assume the packet has been lost and set the data
                //  to go to the UART to "?".  If neither of these is true, keep looping.
                if((nrf24l01_irq_pin_active()&& nrf24l01_irq_rx_dr_active())) //
                {
                    nrf24l01_read_rx_payload(data, strlen((const char*)data)); //get the payload into data
                    break;
                }

                //if loop is on its last iteration, assume packet has been lost.
                if(count == 19999)
                    data = "??";
            }

            nrf24l01_irq_clear_all(); //clear interrupts again
           putsUart0((char*)data); //print the received data (or ? if none) to the screen

            waitMicrosecond(130); //wait for receiver to come from standby to RX
            nrf24l01_set_as_tx(); //resume normal operation as a TX

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


