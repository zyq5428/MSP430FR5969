//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//�о�԰����
//���̵�ַ��http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  �� �� ��   : main.c
//  �� �� ��   : v2.0
//  ��    ��   : HuangKai
//  ��������   : 2014-0101
//  ����޸�   : 
//  ��������   : OLED IIC�ӿ���ʾ����(51ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND    ��Դ��
//              VCC  ��5V��3.3v��Դ
//              D0   P1^0��SCL��
//              D1   P1^1��SDA��
//              RES  �Ӹ�
//              DC   �ӵ�
//              CS   �ӵ�                
//              ----------------------------------------------------------------
// �޸���ʷ   :
// ��    ��   : 
// ��    ��   : HuangKai
// �޸�����   : �����ļ�
//��Ȩ���У�����ؾ���
//Copyright(C) �о�԰����2014/3/16
//All rights reserved
//******************************************************************************/



#include "oled.h"
#include "delay.h"
#include "bmp.h"

/*******************************Main*******************************/
void oled_display_memu(void)
{
    OLED_Init();
    delay_ms(10);

    OLED_ShowCHinese(12,0,0);//��
    OLED_ShowCHinese(40,0,1);//ʪ
    OLED_ShowCHinese(70,0,2);//��
    OLED_ShowCHinese(98,0,3);//��

    OLED_ShowCHinese(0,3,0);//��
    OLED_ShowCHinese(20,3,2);//��
//        OLED_ShowString(56,3,":");
//    OLED_ShowString(48,3,"26.50");
    OLED_ShowCHinese(96,3,4);//��

    OLED_ShowCHinese(0,5,1);//ʪ
    OLED_ShowCHinese(20,5,2);//��
//        OLED_ShowString(56,5,":");
//    OLED_ShowString(48,5,"78.23");
    OLED_ShowString(96,5,"%RH");
//    delay_s(1);
//
//    OLED_ShowString(0,2,"0.96' OLED TEST");
//
//    OLED_ShowString(20,4,"2014/05/01");
//    OLED_ShowString(0,6,"ASCII:");
//    OLED_ShowString(63,6,"CODE:");
//    OLED_ShowChar(48,6,t);//��ʾASCII�ַ�
//    t++;
//    if(t>'~')t=' ';
//    OLED_ShowNum(103,6,t,3,16);//��ʾASCII�ַ�����ֵ
//
//
//    delay_ms(50);
//    OLED_Clear();
//    delay_ms(50);
////        OLED_DrawBMP(0,0,128,8,BMP1);  //ͼƬ��ʾ(ͼƬ��ʾ���ã����ɵ��ֱ�ϴ󣬻�ռ�ý϶�ռ䣬FLASH�ռ�8K��������)
//    delay_ms(50);
//    OLED_DrawBMP(0,0,128,8,gImage_128x64);
//    delay_ms(50);
}

