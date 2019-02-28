/*
 * uart.c
 *
 *  Created on: 2019Äê2ÔÂ27ÈÕ
 *      Author: yqzh02
 */

#include "uart.h"
#include "delay.h"

void uart_pin_set(void)
{
    // Configure UART pins
    //Set P2.0 and P2.1 as Secondary Module Function Input.
    /*

    * Select Port 2d
    * Set Pin 0, 1 to input Secondary Module Function, (UCA0TXD/UCA0SIMO, UCA0RXD/UCA0SOMI).
    */
    GPIO_setAsPeripheralModuleFunctionInputPin(
    GPIO_PORT_P2,
    GPIO_PIN0 + GPIO_PIN1,
    GPIO_SECONDARY_MODULE_FUNCTION
    );
}

uint8_t *UART_TX_Data;      // Pointer to TX data
uint8_t UART_Data_Position;  // TX data Position
uint8_t UART_TX_ByteCtr;      // TX byte count

uint8_t *UART_RX_Data;      // Pointer to RX data
uint8_t UART_RX_ByteCtr;      // RX byte count

void uart_init(void)
{
    // Configure UART
    EUSCI_A_UART_initParam param = {0};
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_ACLK;
    param.clockPrescalar = 3;
    param.firstModReg = 0;
    param.secondModReg = 92;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling = EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;

    if (STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A0_BASE, &param)) {
        return;
    }

    EUSCI_A_UART_enable(EUSCI_A0_BASE);

    EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE,
      EUSCI_A_UART_RECEIVE_INTERRUPT);

    // Enable USCI_A0 RX interrupt
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE,
      EUSCI_A_UART_RECEIVE_INTERRUPT);                     // Enable interrupt

    __enable_interrupt();

//    uint8_t i = 0;
//    while (1)
//    {
//        i = i+1;                      // Increment TX data
//        // Load data onto buffer
//        EUSCI_A_UART_transmitData(EUSCI_A0_BASE, 5);
//        delay_s(1);
//    }
}

uint8_t uart_multibyte_send(uint8_t data[], uint8_t num)
{
    uint8_t i;

    for (i = 0; i < num; i++ ) {
        EUSCI_A_UART_transmitData(EUSCI_A0_BASE, data[i]);
    }

    return 0;
}


//******************************************************************************
//
//This is the USCI_A0 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A0_VECTOR)))
#endif
void USCI_A0_ISR(void)
{
  switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
      UART_RX_Data[0] = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);

      EUSCI_A_UART_transmitData(EUSCI_A0_BASE, UART_RX_Data[0]);
      break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}
