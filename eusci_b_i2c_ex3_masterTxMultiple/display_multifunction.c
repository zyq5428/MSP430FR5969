/*
 * display_multifunction.c
 *
 *  Created on: 2019Äê2ÔÂ27ÈÕ
 *      Author: yqzh02
 */

#include "display_multifunction.h"
#include "uart.h"
#include "sht3x.h"
#include "oled.h"

unsigned char tem_title[] = "\t\nTemperature is :\t\n";
unsigned char hum_title[] = "\t\nhumidity is :\t\n";

unsigned char tem_suffix[] = " ¡ãC";
unsigned char hum_suffix[] = " %RH";

unsigned char display(void *handle, void *ptr, unsigned char type)
{
    float Tem = ((SHT3x_Sensor_Param *)ptr)->Temperature;
    float hum = ((SHT3x_Sensor_Param *)ptr)->Humidity;

    int tmp;
    unsigned char tem_unit[5], hum_unit[5];

    tmp = (int)(Tem * 100);

    tem_unit[4] = tmp / 1 % 10 + '0';
    tem_unit[3] = tmp / 10 % 10 + '0';
    tem_unit[2] = '.';
    tem_unit[1] = tmp / 100 % 10 + '0';
    tem_unit[0] = tmp / 1000 % 10 + '0';

    tmp = (int)(hum * 100);

    hum_unit[4] = tmp / 1 % 10 + '0';
    hum_unit[3] = tmp / 10 % 10 + '0';
    hum_unit[2] = '.';
    hum_unit[1] = tmp / 100 % 10 + '0';
    hum_unit[0] = tmp / 1000 % 10 + '0';


    uart_multibyte_send(tem_title, sizeof(tem_title));
    uart_multibyte_send(tem_unit, sizeof(tem_unit));
    uart_multibyte_send(tem_suffix, sizeof(tem_suffix));

    OLED_ShowString_N(48,3, tem_unit, sizeof(tem_unit));

    uart_multibyte_send(hum_title, sizeof(hum_title));
    uart_multibyte_send(hum_unit, sizeof(hum_unit));
    uart_multibyte_send(hum_suffix, sizeof(hum_suffix));

    OLED_ShowString_N(48,5, hum_unit, sizeof(hum_unit));

    return 0;
}
