/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
#include "driverlib.h"
#include "cs.h"
#include "delay.h"
#include "i2c.h"

//******************************************************************************
//! This example shows how to configure the I2C module as a master for
//! multi byte transmission in interrupt driven mode. The address of the slave
//! module is set in this example.
//!
//!  Demo - EUSCI_B0 I2C Master TX multiple bytes to MSP430 Slave
//!
//!  Description: This demo connects two MSP430's via the I2C bus. The master
//!  transmits to the slave. This is the MASTER CODE. It cntinuously
//!  transmits an array of data and demonstrates how to implement an I2C
//!  master transmitter sending multiple bytes using the USCI_B0 TX interrupt.
//!  ACLK = n/a, MCLK = SMCLK = BRCLK = default DCO = ~1.045MHz
//!
//!                                /|\  /|\
//!                MSP430FR5969    10k  10k      MSP430FR5969
//!                   slave         |    |         master
//!             -----------------   |    |   -----------------
//!           -|XIN  P1.6/UCB0SDA|<-|----+->|P1.6/UCB0SDA  XIN|-
//!            |                 |  |       |                 |
//!           -|XOUT             |  |       |             XOUT|-
//!            |     P1.7/UCB0SCL|<-+------>|P1.7/UCB0SCL     |
//!            |                 |          |                 |
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - I2C peripheral
//! - GPIO Port peripheral (for I2C pins)
//! - SCL2
//! - SDA
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - USCI_B0_VECTOR.
//

//******************************************************************************
//*****************************************************************************
//
//Set the address for slave module. This is a 7-bit address sent in the
//following format:
//[A6:A5:A4:A3:A2:A1:A0:RS]
//
//A zero in the "RS" position of the first byte means that the master
//transmits (sends) data to the selected slave, and a one in this position
//means that the master receives data from the slave.
//
//*****************************************************************************
//#define SLAVE_ADDRESS 0x48
#define SLAVE_ADDRESS 0x44

//uint8_t TXData =0;                    // Pointer to TX data
uint8_t TXData[2] = {0x2c, 0x06};                    // Pointer to TX data
uint8_t Data_Position = 0;                    // Pointer to TX data
uint8_t TXByteCtr;

uint8_t RXData[6];
#define RXCOUNT 0x06



void main(void)
{
    WDT_A_hold(WDT_A_BASE);

    cs_set();

    i2c_pin_set();

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();


    while(1)
    {
        EUSCI_B_I2C_initMasterParam param = {0};
        param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
        param.i2cClk = CS_getSMCLK();
        param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
        param.byteCounterThreshold = 0;
        param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
        EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &param);

        //Specify slave address
        EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE,
                    SLAVE_ADDRESS
                    );

        //Set Master in receive mode
        EUSCI_B_I2C_setMode(EUSCI_B0_BASE,
                    EUSCI_B_I2C_TRANSMIT_MODE
                    );

        //Enable I2C Module to start operations
        EUSCI_B_I2C_enable(EUSCI_B0_BASE);

        EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
                    EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
                    EUSCI_B_I2C_NAK_INTERRUPT
                    );

        //Enable master Receive interrupt
        EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
                    EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
                    EUSCI_B_I2C_NAK_INTERRUPT
                  );

    delay_us(1000);                   // Delay between transmissions
    TXByteCtr = 2;                          // Load TX byte counter
    Data_Position = 0;

    while (EUSCI_B_I2C_SENDING_STOP == EUSCI_B_I2C_masterIsStopSent
    		(EUSCI_B0_BASE));

    EUSCI_B_I2C_masterSendMultiByteStart(EUSCI_B0_BASE,
    		TXData[Data_Position++]);

    __bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
    										// Remain in LPM0 until all data
    										// is TX'd
    										// Increment data byte

    delay_us(5000);

    EUSCI_B_I2C_initMasterParam rx_param = {0};
    rx_param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    rx_param.i2cClk = CS_getSMCLK();
    rx_param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
    rx_param.byteCounterThreshold = RXCOUNT;
    rx_param.autoSTOPGeneration = EUSCI_B_I2C_SEND_STOP_AUTOMATICALLY_ON_BYTECOUNT_THRESHOLD;
    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &rx_param);

    //Specify slave address
    EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE,
        SLAVE_ADDRESS
        );

    //Set Master in receive mode
    EUSCI_B_I2C_setMode(EUSCI_B0_BASE,
        EUSCI_B_I2C_RECEIVE_MODE
        );

    //Enable I2C Module to start operations
    EUSCI_B_I2C_enable(EUSCI_B0_BASE);

    EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
        EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
        EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT +
        EUSCI_B_I2C_NAK_INTERRUPT
        );

    //Enable master Receive interrupt
    EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
        EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
        EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT +
        EUSCI_B_I2C_NAK_INTERRUPT
        );

    delay_us(2000);
    Data_Position = 0;

    while (EUSCI_B_I2C_SENDING_STOP ==
            EUSCI_B_I2C_masterIsStopSent(EUSCI_B0_BASE));

    EUSCI_B_I2C_masterReceiveStart(EUSCI_B0_BASE);

      // Enter LPM0 w/ interrupt
      __bis_SR_register(CPUOFF+GIE);

    }
}

//------------------------------------------------------------------------------
// The USCIAB0TX_ISR is structured such that it can be used to transmit any
// number of bytes by pre-loading TXByteCtr with the byte count. Also, TXData
// points to the next byte to transmit.
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCIB0_ISR(void)
{
    static uint8_t count = 0;
    switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
  {
        case USCI_NONE:             // No interrupts break;
            break;
        case USCI_I2C_UCALIFG:      // Arbitration lost
            break;
        case USCI_I2C_UCNACKIFG:    // NAK received (master only)
            //resend start if NACK
            EUSCI_B_I2C_masterSendStart(EUSCI_B0_BASE);
            break;
        case USCI_I2C_UCSTTIFG:     // START condition detected with own address (slave mode only)
            break;
        case USCI_I2C_UCSTPIFG:     // STOP condition detected (master & slave mode)
            break;
        case USCI_I2C_UCRXIFG3:     // RXIFG3
            break;
        case USCI_I2C_UCTXIFG3:     // TXIFG3
            break;
        case USCI_I2C_UCRXIFG2:     // RXIFG2
            break;
        case USCI_I2C_UCTXIFG2:     // TXIFG2
            break;
        case USCI_I2C_UCRXIFG1:     // RXIFG1
            break;
        case USCI_I2C_UCTXIFG1:     // TXIFG1
            break;
        case USCI_I2C_UCRXIFG0:     // RXIFG0
            // Get RX data
            RXData[Data_Position++] = EUSCI_B_I2C_masterReceiveSingle(
                                    EUSCI_B0_BASE
                    );
          if (++count >= RXCOUNT) {
              count = 0;
              __bic_SR_register_on_exit(CPUOFF); // Exit LPM0
          }
            break; // Vector 24: RXIFG0 break;
        case USCI_I2C_UCTXIFG0:     // TXIFG0
            // Check TX byte counter
            if (TXByteCtr)
            {
                EUSCI_B_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, TXData[Data_Position++]);
                // Decrement TX byte counter
                TXByteCtr--;
            }
            else
            {
                EUSCI_B_I2C_masterSendMultiByteStop(EUSCI_B0_BASE);
                // Exit LPM0
                __bic_SR_register_on_exit(CPUOFF);
            }
            break;
        case USCI_I2C_UCBCNTIFG:    // Byte count limit reached (UCBxTBCNT)
            GPIO_toggleOutputOnPin(
                GPIO_PORT_P1,
                GPIO_PIN0
                );
            break;
        case USCI_I2C_UCCLTOIFG:    // Clock low timeout - clock held low too long
            break;
        case USCI_I2C_UCBIT9IFG:    // Generated on 9th bit of a transmit (for debugging)
            break;
        default:
            break;
  }
}

