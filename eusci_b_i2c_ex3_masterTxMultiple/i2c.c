/*
 * i2c.c
 *
 *  Created on: 2019Äê2ÔÂ26ÈÕ
 *      Author: yqzh02
 */

#include "i2c.h"
#include "delay.h"

uint8_t SLAVE_ADDRESS;

void i2c_pin_set(void)
{
    // Configure Pins for I2C
    //Set P1.6 and P1.7 as Secondary Module Function Input.
    /*

    * Select Port 1
    * Set Pin 6, 7 to input Secondary Module Function, (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
    */
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P1,
        GPIO_PIN6 + GPIO_PIN7,
        GPIO_SECONDARY_MODULE_FUNCTION
    );
}

void i2c_addr_set(uint8_t addr)
{
    SLAVE_ADDRESS = addr;
}

uint8_t *I2C_TX_Data;      // Pointer to TX data
uint8_t I2C_Data_Position;  // TX data Position
uint8_t I2C_TX_ByteCtr;      // TX byte count

int i2c_write(uint8_t addr, uint8_t data[], uint8_t num)
{
    I2C_TX_Data = data;
    I2C_TX_ByteCtr = num - 1;
    SLAVE_ADDRESS = addr;

    EUSCI_B_I2C_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = CS_getSMCLK();
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
    param.byteCounterThreshold = 0;
    param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &param);

    //Specify slave address
    EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE, SLAVE_ADDRESS);

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

    //Delay between each transaction
    __delay_cycles(100);

    I2C_Data_Position = 0;

    while (EUSCI_B_I2C_SENDING_STOP == EUSCI_B_I2C_masterIsStopSent(EUSCI_B0_BASE))
        ;

    EUSCI_B_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, I2C_TX_Data[I2C_Data_Position++]);

    __bis_SR_register(CPUOFF + GIE);        // Enter LPM0 w/ interrupts
                                            // Remain in LPM0 until all data
                                            // is TX'd
                                            // Increment data byte
    return 0;
}


int i2c_write_address_already(uint8_t data[], uint8_t num)
{
    i2c_write(SLAVE_ADDRESS, data, num);
    return 0;
}


uint8_t *I2C_RX_Data;        // Pointer to RX data
uint8_t I2C_RX_ByteCtr;      // RX byte count

int i2c_read(uint8_t addr, uint8_t data[], uint8_t num)
{
    I2C_RX_Data = data;
    I2C_RX_ByteCtr = num;
    SLAVE_ADDRESS = addr;

    EUSCI_B_I2C_initMasterParam rx_param = {0};
    rx_param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    rx_param.i2cClk = CS_getSMCLK();
//    rx_param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_400KBPS;
    rx_param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_100KBPS;
    rx_param.byteCounterThreshold = I2C_RX_ByteCtr;
    rx_param.autoSTOPGeneration = EUSCI_B_I2C_SEND_STOP_AUTOMATICALLY_ON_BYTECOUNT_THRESHOLD;
    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &rx_param);

    //Specify slave address
    EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE, SLAVE_ADDRESS);

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

    I2C_Data_Position = 0;

    while (EUSCI_B_I2C_SENDING_STOP ==
            EUSCI_B_I2C_masterIsStopSent(EUSCI_B0_BASE));

    EUSCI_B_I2C_masterReceiveStart(EUSCI_B0_BASE);

      // Enter LPM0 w/ interrupt
      __bis_SR_register(CPUOFF+GIE);

    return 0;
}

int i2c_read_address_already(uint8_t data[], uint8_t num)
{
    i2c_read(SLAVE_ADDRESS, data, num);
    return 0;
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
            I2C_RX_Data[I2C_Data_Position++] = EUSCI_B_I2C_masterReceiveSingle(
                                    EUSCI_B0_BASE
                    );
          if (++count >= I2C_RX_ByteCtr) {
              count = 0;
              __bic_SR_register_on_exit(CPUOFF); // Exit LPM0
          }
            break; // Vector 24: RXIFG0 break;
        case USCI_I2C_UCTXIFG0:     // TXIFG0
            // Check TX byte counter
            if (I2C_TX_ByteCtr)
            {
                EUSCI_B_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, I2C_TX_Data[I2C_Data_Position++]);
                // Decrement TX byte counter
                I2C_TX_ByteCtr--;
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
