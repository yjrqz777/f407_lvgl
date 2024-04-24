
#include "st7789v.h"
#include "font.h"
//#include "FreeRTOS.h"
//#include "task.h"
#include "spi.h"
#include "lv_port_disp.h"
#include <stdlib.h>
/**
 * @brief    SPI 发送字节函数
 * @param    TxData    要发送的数据
 * @param    size    发送数据的字节大小
 * @return  0:写入成功,其他:写入失败
 */
uint8_t SPI_WriteByte(uint8_t *TxData,uint16_t size)
{
//		printf("%x\n",*TxData);
    return HAL_SPI_Transmit(&hspi1,TxData,size,1000);
//	return HAL_SPI_Transmit_DMA(&hspi1,TxData,size);
}

/**
 * @brief   写命令到LCD
 * @param   cmd —— 需要发送的命令
 * @return  none
 */
static void LCD_Write_Cmd(uint8_t cmd)
{
		LCD_DC(0);
    SPI_WriteByte(&cmd, 1);
}


/**
 * @brief   写数据到LCD
 * @param   dat —— 需要发送的数据
 * @return  none
 */
static void LCD_Write_Data(uint8_t dat)
{
    LCD_DC(1);
    SPI_WriteByte(&dat, 1);
}



void LCD_Write_Data2Bytes(uint16_t dat)
{
    LCD_DC(1);
		uint8_t dat_temp = dat>>8;
		uint8_t dat_temp2 = dat;
    SPI_WriteByte(&dat_temp, 1);
		SPI_WriteByte(&dat_temp2, 1);
}




/**
 * @brief   设置数据写入LCD显存区域
 * @param   x1,y1    —— 起点坐标
 * @param   x2,y2    —— 终点坐标
 * @return  none
 */
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    /* 指定X方向操作区域 */
    LCD_Write_Cmd(0x2a);
//	LCD_Write_Data2Bytes(x1);
//	LCD_Write_Data2Bytes(x2);
    LCD_Write_Data(x1 >> 8);
    LCD_Write_Data(x1);
    LCD_Write_Data(x2 >> 8);
    LCD_Write_Data(x2);

    /* 指定Y方向操作区域 */
    LCD_Write_Cmd(0x2b);
//	LCD_Write_Data2Bytes(y1);
//	LCD_Write_Data2Bytes(y2);
    LCD_Write_Data(y1 >> 8);
    LCD_Write_Data(y1);
    LCD_Write_Data(y2 >> 8);
    LCD_Write_Data(y2);

    /* 发送该命令，LCD开始等待接收显存数据 */
    LCD_Write_Cmd(0x2C);
}







void ST7789V_SetDir(uint8_t Dir_Mode)
{
	LCD_Write_Cmd(0x36);/*显示方向*/ 
	if(Dir_Mode==0)LCD_Write_Data(0x00);
	else if(Dir_Mode==1)LCD_Write_Data(0xC0);
	else if(Dir_Mode==2)LCD_Write_Data(0x70);
	else LCD_Write_Data(0xA0);
}


/**
 * @brief   以一种颜色清空LCD屏
 * @param   color —— 清屏颜色(16bit)
 * @return  none
 */
void LCD_Clear(uint16_t color)
{
	

    uint32_t buf_size = (320 * 280);
    uint8_t *buf = (uint8_t*)malloc(buf_size*2);
//		printf("-%d-%d-%d-%d-w=%d,,h=%d\n", x1, y1, x2, y2,width,height);
		HAL_Delay(1);
    if(buf == NULL)
    {
        // 处理内存分配失败的情况
        return;
    }

		LCD_Address_Set(0, 0, 320, 280);
		LCD_DC(1);
//		printf("-1--%d-%d-%d---\n",width,height,buf_size);
    for(uint32_t i = 0; i < buf_size; i++)
    {
			
        buf[i*2] = (color>> 8)& 0xFF;
        buf[i*2 + 1] = color& 0xFF;

    }

    HAL_SPI_Transmit(&hspi1, buf,buf_size*2,1000);
////			HAL_SPI_Transmit(&hspi1,(uint8_t *)color,buf_size*2,1000);
//			HAL_SPI_Transmit_DMA(&hspi1,buf,buf_size*2);
    // 指定显存操作地址为指定区域
    free(buf);
}



void LCD_color_fill_lvgl(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lv_color_t * color)
{
	
    uint16_t width = (x2 - x1)+1;
    uint16_t height = (y2 - y1)+1;
    uint32_t buf_size = (width * height); 
    uint8_t *buf = (uint8_t*)malloc(buf_size*2);
//		printf("-%d-%d-%d-%d-w=%d,,h=%d\n", x1, y1, x2, y2,width,height);
		HAL_Delay(1);
    if(buf == NULL)
    {
        // 处理内存分配失败的情况
        return;
    }

		LCD_Address_Set(x1, y1, x2, y2);
		LCD_DC(1);
//		printf("-1--%d-%d-%d---\n",width,height,buf_size);
    for(uint32_t i = 0; i < buf_size; i++)
    {
			
        buf[i*2] = (color->full >> 8)& 0xFF;
        buf[i*2 + 1] = color->full& 0xFF;
				color++;

    }

//    HAL_SPI_Transmit(&hspi1, buf,buf_size*2,1000);
			HAL_SPI_Transmit_DMA(&hspi1,buf,buf_size*2);
    free(buf);
}

void LCD_color_fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t width = x2 - x1;
    uint16_t height = y2 - y1;
    uint32_t buf_size = width * height*2; // 16-bit color, 2 bytes per pixel
    uint8_t *buf = (uint8_t*)malloc(buf_size);

		LCD_DC(1);
		LCD_Address_Set(x1, y1, x2, y2);
		HAL_SPI_Transmit(&hspi1,buf,buf_size*2,1000);

    free(buf);
}


void LCD_color_point(uint16_t x1, uint16_t y1,uint16_t color)
{

    LCD_Address_Set(x1, y1, x1, y1);
		LCD_Write_Data2Bytes(color);
		
}









void lcd_test()
{
		LCD_color_fill(0,0,320,280,BLACK);
//		LCD_color_fill(0,0,40,40,RED);
		LCD_color_fill(40,40,100,100,RED);
		
		LCD_color_fill(100,100,180,180,BLUE);
}



#define DUBEG 0



/**
 * @brief   LCD初始化
 * @param   none
 * @return  none
 */


void st7789v_init()
{
		LCD_BLK(0);
		LCD_CS(0);
    HAL_Delay(100);
//    
//		HAL_Delay();
		LCD_RST(1);
		HAL_Delay(100);
		LCD_RST(0);
		HAL_Delay(100);
		LCD_RST(1);
		HAL_Delay(130);
    /* 初始化和LCD通信的引脚 */
//    HAL_Delay(120);

    /* 关闭睡眠模式 */
    LCD_Write_Cmd(0x11);
    HAL_Delay(120);
		ST7789V_SetDir(USE_HORIZONTAL);
//    LCD_Write_Cmd(0x36);
//    LCD_Write_Data(0x00);
	
	
#if DUBEG
	
	
	LCD_Write_Cmd(0x3A);
	LCD_Write_Data(0x55);
//-------------ST7789V Frame rate setting-----------//
	LCD_Write_Cmd(0xB2);//Porch Setting
	LCD_Write_Data(0x0C);
	LCD_Write_Data(0x0C);
	LCD_Write_Data(0x00);
	LCD_Write_Data(0x33);
	LCD_Write_Data(0x33); 
	
	LCD_Write_Cmd(0xB7);//Gate Control
	LCD_Write_Data(0x75);//······12.2v   -10.43v  
//--------------ST7789V Power setting---------------//
	LCD_Write_Cmd(0xC2);//VDV and VRH Command Enable
	LCD_Write_Data(0x01);

	LCD_Write_Cmd(0xC3);//VRH Set
	LCD_Write_Data(0x16);//······4.3+( vcom+vcom offset+vdv)   

	LCD_Write_Cmd(0xC4);//VDV Set
	LCD_Write_Data(0x20); //0v 

	LCD_Write_Cmd(0xC6);//Frame Rate Control in Normal Mode 
	LCD_Write_Data(0x0F);//111Hz   

	LCD_Write_Cmd(0xD0);//Power Control 1 
	LCD_Write_Data(0xA4);
	LCD_Write_Data(0xA1);
	
	LCD_Write_Cmd(0xD6);
	LCD_Write_Data(0xA1);
	
	LCD_Write_Cmd(0xBB);//VCOM
	LCD_Write_Data(0x3B);
//---------------ST7789V gamma setting-------------//
	LCD_Write_Cmd(0xE0);//Set Gamma
	LCD_Write_Data(0xF0);
	LCD_Write_Data(0x0B);
	LCD_Write_Data(0x11);
	LCD_Write_Data(0x0E);
	LCD_Write_Data(0x0D);
	LCD_Write_Data(0x19);
	LCD_Write_Data(0x36);
	LCD_Write_Data(0x33);
	LCD_Write_Data(0x4B);
	LCD_Write_Data(0x07);
	LCD_Write_Data(0x14);
	LCD_Write_Data(0x14);
	LCD_Write_Data(0x2C);
	LCD_Write_Data(0x2E);    

	LCD_Write_Cmd(0xE1);//Set Gamma
	LCD_Write_Data(0xF0);
	LCD_Write_Data(0x0D);
	LCD_Write_Data(0x12);
	LCD_Write_Data(0x0B);
	LCD_Write_Data(0x09);
	LCD_Write_Data(0x03);
	LCD_Write_Data(0x32);
	LCD_Write_Data(0x44);
	LCD_Write_Data(0x48);
	LCD_Write_Data(0x39);
	LCD_Write_Data(0x16);
	LCD_Write_Data(0x16);
	LCD_Write_Data(0x2d);
	LCD_Write_Data(0x30);

	LCD_Write_Cmd(0xE4);
	LCD_Write_Data(0x25);
	LCD_Write_Data(0x00);
	LCD_Write_Data(0x00);

	LCD_Write_Cmd(0x21);//此命令用于从显示反转模式恢复。

	LCD_Write_Cmd(0x29);//显示打开
	
	LCD_Write_Cmd(0x2c);
	
	
	
	
#endif
	
	

#if DUBEG
	

    /* 开始设置显存扫描模式，数据格式等 */
    LCD_Write_Cmd(0x36);
    LCD_Write_Data(0x00);
    /* RGB 5-6-5-bit格式  */
    LCD_Write_Cmd(0x3A);
    LCD_Write_Data(0x55);
    /* porch 设置 */
    LCD_Write_Cmd(0xB2);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x33);
    LCD_Write_Data(0x33);
    /* VGH设置 */
    LCD_Write_Cmd(0xB7);
    LCD_Write_Data(0x72);
    /* VCOM 设置 */
    LCD_Write_Cmd(0xBB);
    LCD_Write_Data(0x3D);
    /* LCM 设置 */
    LCD_Write_Cmd(0xC0);
    LCD_Write_Data(0x2C);
    /* VDV and VRH 设置 */
    LCD_Write_Cmd(0xC2);
    LCD_Write_Data(0x01);
    /* VRH 设置 */
    LCD_Write_Cmd(0xC3);
    LCD_Write_Data(0x19);
    /* VDV 设置 */
    LCD_Write_Cmd(0xC4);
    LCD_Write_Data(0x20);
    /* 普通模式下显存速率设置 60Mhz */
    LCD_Write_Cmd(0xC6);
    LCD_Write_Data(0x0F);
    /* 电源控制 */
    LCD_Write_Cmd(0xD0);
    LCD_Write_Data(0xA4);
    LCD_Write_Data(0xA1);
    /* 电压设置 */
    LCD_Write_Cmd(0xE0);
    LCD_Write_Data(0xD0);
    LCD_Write_Data(0x04);
    LCD_Write_Data(0x0D);
    LCD_Write_Data(0x11);
    LCD_Write_Data(0x13);
    LCD_Write_Data(0x2B);
    LCD_Write_Data(0x3F);
    LCD_Write_Data(0x54);
    LCD_Write_Data(0x4C);
    LCD_Write_Data(0x18);
    LCD_Write_Data(0x0D);
    LCD_Write_Data(0x0B);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x23);
    /* 电压设置 */
    LCD_Write_Cmd(0xE1);
    LCD_Write_Data(0xD0);
    LCD_Write_Data(0x04);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x11);
    LCD_Write_Data(0x13);
    LCD_Write_Data(0x2C);
    LCD_Write_Data(0x3F);
    LCD_Write_Data(0x44);
    LCD_Write_Data(0x51);
    LCD_Write_Data(0x2F);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x20);
    LCD_Write_Data(0x23);
    /* 显示开 */
    LCD_Write_Cmd(0x21);
    LCD_Write_Cmd(0x29);






#endif   





LCD_Write_Cmd(0x3a);
LCD_Write_Data(0x05);//0x55
//--------------------------------ST7789V Frame rate setting-----------------

LCD_Write_Cmd(0xb2);
LCD_Write_Data(0x0c);
LCD_Write_Data(0x0c);
LCD_Write_Data(0x00);
LCD_Write_Data(0x33);
LCD_Write_Data(0x33);
LCD_Write_Cmd(0xb7);
LCD_Write_Data(0x35);
//---------------------------------ST7789V Power setting---------------------

LCD_Write_Cmd(0xbb);
LCD_Write_Data(0x19);//0x28
LCD_Write_Cmd(0xc0);
LCD_Write_Data(0x2c);
LCD_Write_Cmd(0xc2);
LCD_Write_Data(0x01);
LCD_Write_Cmd(0xc3);
LCD_Write_Data(0x12);//0x10
LCD_Write_Cmd(0xc4);
LCD_Write_Data(0x20);
LCD_Write_Cmd(0xc6);
LCD_Write_Data(0x0f);
LCD_Write_Cmd(0xd0);
LCD_Write_Data(0xa4);
LCD_Write_Data(0xa1);
//--------------------------------ST7789V gamma setting----------------------

LCD_Write_Cmd(0xe0);
LCD_Write_Data(0xd0);
LCD_Write_Data(0x04);//0x00
LCD_Write_Data(0x0d);//0x02
LCD_Write_Data(0x11);//0x07
LCD_Write_Data(0x13);//0x0a
LCD_Write_Data(0x2b);//0x28
LCD_Write_Data(0x3f);//0x32
LCD_Write_Data(0x54);//0x44
LCD_Write_Data(0x4c);//0x42
LCD_Write_Data(0x18);//0x06
LCD_Write_Data(0x0d);//0x0e
LCD_Write_Data(0x0b);//0x12
LCD_Write_Data(0x1f);//0x14
LCD_Write_Data(0x23);//0x17
LCD_Write_Cmd(0xe1);
LCD_Write_Data(0xd0);//0xd0
LCD_Write_Data(0x04);//0x00
LCD_Write_Data(0x0c);//0x02
LCD_Write_Data(0x11);//0x07
LCD_Write_Data(0x13);//0x0a
LCD_Write_Data(0x2c);//0x28
LCD_Write_Data(0x3f);//0x31
LCD_Write_Data(0x44);//0x54
LCD_Write_Data(0x51);//0x47
LCD_Write_Data(0x2f);//0x0e
LCD_Write_Data(0x1f);//0x1c
LCD_Write_Data(0x1f);//0x17
LCD_Write_Data(0x20);//0x1b
LCD_Write_Data(0x23);//0x1e



LCD_Write_Cmd(0x20);

LCD_Write_Cmd(0x29);


		LCD_BLK(1);
    /* 清屏为白色 */
    LCD_Clear(WHITE);
	  LCD_Clear(GREEN);
//    LCD_color_point(40,40,RED);
//		HAL_Delay(1000);
		lcd_test();
#if DUBEG

		lcd_test();
		
#endif
		

}






