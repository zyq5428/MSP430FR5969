/*
 * i2c.h
 *
 *  Created on: 2019Äê2ÔÂ26ÈÕ
 *      Author: yqzh02
 */

#ifndef I2C_H_
#define I2C_H_

#include "driverlib.h"

extern void i2c_pin_set(void);
extern void i2c_addr_set(uint8_t addr);

extern int i2c_write(uint8_t addr, uint8_t data[], uint8_t num);
extern int i2c_read(uint8_t addr, uint8_t data[], uint8_t num);

extern int i2c_write_address_already(uint8_t data[], uint8_t num);
extern int i2c_read_address_already(uint8_t data[], uint8_t num);

#endif /* I2C_H_ */
