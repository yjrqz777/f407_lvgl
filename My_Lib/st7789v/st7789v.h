
/**************����BվUP��Sangk-Hu****************/

/*****************QQȺ��690367095******************/

/****��עUP��˽�Ż��߼�Ⱥ������ϻ�����������*****/

/********************************************/
/*���þ�ȷ��ʱ��ʹ��SysTick_Init��ʼ��ʱ����*/
/********************************************/
#ifndef _st7789v_H
#define _st7789v_H
#include "main.h"
#include "spi.h"
//#include "tim.h"
//#include "usart.h"
#include "gpio.h"
#include "lv_port_disp.h"
//#include <stm32f40x.h>
//#include "bit.h"     


#define LCD_RST(n) (n?\
											HAL_GPIO_WritePin(lcd_rest_GPIO_Port,lcd_rest_Pin,GPIO_PIN_SET):\
											HAL_GPIO_WritePin(lcd_rest_GPIO_Port,lcd_rest_Pin,GPIO_PIN_RESET)\
										)
//#define LCD_BLK(n) (n?\
//											HAL_GPIO_WritePin(lcd_blk_GPIO_Port,lcd_blk_Pin,GPIO_PIN_SET):\
//											HAL_GPIO_WritePin(lcd_blk_GPIO_Port,lcd_blk_Pin,GPIO_PIN_RESET)\
//										)
										
										
#define LCD_DC(n) (n?\
											HAL_GPIO_WritePin(lcd_dc_GPIO_Port,lcd_dc_Pin,GPIO_PIN_SET):\
											HAL_GPIO_WritePin(lcd_dc_GPIO_Port,lcd_dc_Pin,GPIO_PIN_RESET)\
										)
										
#define LCD_CS(n) (n?\
											HAL_GPIO_WritePin(lcd_cs_GPIO_Port,lcd_cs_Pin,GPIO_PIN_SET):\
											HAL_GPIO_WritePin(lcd_cs_GPIO_Port,lcd_cs_Pin,GPIO_PIN_RESET)\
										)



#define WIDTH_OFFSET 		 0
#define HIGH_OFFSET 		 20

#define USE_HORIZONTAL 	 2  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 					 280
#define LCD_H 					 320

#else
#define LCD_W 					 320
#define LCD_H 					 280
#endif

//������ɫ
#define WHITE         	 (uint16_t)0xFFFF
#define BLACK         	 (uint16_t)0x0000	  
#define BLUE           	 (uint16_t)0x001F  
#define BRED             (uint16_t)0XF81F
#define GRED 			       (uint16_t)0XFFE0
#define GBLUE			       (uint16_t)0X07FF
#define RED           	 (uint16_t)0xF800
#define MAGENTA       	 (uint16_t)0xF81F
#define GREEN         	 (uint16_t)0x07E0
#define CYAN          	 (uint16_t)0x7FFF
#define YELLOW        	 (uint16_t)0xFFE0
#define BROWN 			     (uint16_t)0XBC40 //��ɫ
#define BRRED 			     (uint16_t)0XFC07 //�غ�ɫ
#define GRAY  			     (uint16_t)0X8430 //��ɫ
#define DARKBLUE      	 (uint16_t)0X01CF	//����ɫ
#define LIGHTBLUE      	 (uint16_t)0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 (uint16_t)0X5458 //����ɫ
#define LIGHTGREEN     	 (uint16_t)0X841F //ǳ��ɫ
#define LGRAY 			     (uint16_t)0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        (uint16_t)0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           (uint16_t)0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)



extern void st7789v_init();
extern void LCD_color_fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
extern void LCD_color_point(uint16_t x1, uint16_t y1,uint16_t color);
extern void LCD_color_fill_lvgl(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lv_color_t * color);
#endif
