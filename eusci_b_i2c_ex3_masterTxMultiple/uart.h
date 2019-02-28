/*
 * uart.h
 *
 *  Created on: 2019Äê2ÔÂ27ÈÕ
 *      Author: yqzh02
 */

#ifndef UART_H_
#define UART_H_

#include "driverlib.h"

extern void uart_pin_set(void);

extern void uart_init(void);

uint8_t uart_multibyte_send(uint8_t data[], uint8_t num);


#endif /* UART_H_ */
