
#ifndef _st7789v_H
#define _st7789v_H
#include "main.h"
#include "spi.h"
//#include "tim.h"
//#include "usart.h"
#include "gpio.h"
#include "lv_port_disp.h"
#include "stm32f4xx_hal.h"

/*
0 旧
1 新
*/
#define DEVICE_IC 0
#define LCD_BLK(n) HAL_GPIO_WritePin(GPIOE,GPIO_PIN_14,n?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD_CS(n) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,n?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD_RST(n) HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,n?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD_DC(n) HAL_GPIO_WritePin(GPIOE,GPIO_PIN_12,n?GPIO_PIN_SET:GPIO_PIN_RESET)


#define WIDTH_OFFSET 		 0
#define HIGH_OFFSET 		 20

#define USE_HORIZONTAL 	 2  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#if (USE_HORIZONTAL== 0 || USE_HORIZONTAL == 1)
#define LCD_W 					 240
#define LCD_H 					 320

#else
#define LCD_W 					 320
#define LCD_H 					 240
#endif

//画笔颜色
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
#define BROWN 			     (uint16_t)0XBC40 //棕色
#define BRRED 			     (uint16_t)0XFC07 //棕红色
#define GRAY  			     (uint16_t)0X8430 //灰色
#define DARKBLUE      	 (uint16_t)0X01CF	//深蓝色
#define LIGHTBLUE      	 (uint16_t)0X7D7C	//浅蓝色  
#define GRAYBLUE       	 (uint16_t)0X5458 //灰蓝色
#define LIGHTGREEN     	 (uint16_t)0X841F //浅绿色
#define LGRAY 			     (uint16_t)0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        (uint16_t)0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           (uint16_t)0X2B12 //浅棕蓝色(选择条目的反色)



typedef enum eSendModeDef
{
	E_SEND_CMD,
	E_SEND_DATA,
	E_SEND_DATA_MAX
}eSendModeDef;



typedef enum eAsciiFontDef
{
    E_FONT_1206 = 0,
    E_FONT_2412,
	E_FONT_MAX
}eAsciiFontDef;



extern void Drvst7789Init();
extern void DrvSt7789FillLvgl(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lv_color_t *color);

// extern void LCD_color_fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
// extern void LCD_color_point(uint16_t x1, uint16_t y1,uint16_t color);
// extern void LCD_color_fill_lvgl(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lv_color_t * color);
#endif
