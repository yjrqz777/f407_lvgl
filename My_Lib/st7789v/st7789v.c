
#include "st7789v.h"
#include "font.h"
#include "spi.h"
#include <stdlib.h>



static uint8_t DrvSpiSendData(uint8_t *Data, uint16_t Size)
{
    LCD_CS(0);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi2, Data, Size, 1000);
    LCD_CS(1);
    return (status == HAL_OK) ? 1 : 0;
}

static uint8_t DrvSt7789Send(eSendModeDef SendMode, uint8_t Data)
{
    if (SendMode == E_SEND_CMD)
    {
        LCD_DC(0);
    }
    else if (SendMode == E_SEND_DATA)
    {
        LCD_DC(1);
    }
    DrvSpiSendData(&Data, 1);
}

static void DrvSt7789SendU16(uint16_t Data)
{
    uint8_t Data16[2] = {Data >> 8, Data & 0xFF};
    LCD_DC(1);
    DrvSpiSendData(Data16, 2);
}


void DrvSt7789SetDir(uint8_t dir_mode)
{
    static const uint8_t madctl_table[4] = {0x00, 0xC0, 0x70, 0xA0};

    if (dir_mode > 3)
        dir_mode = 0;

    DrvSt7789Send(E_SEND_CMD,0x36);
    DrvSt7789Send(E_SEND_DATA, madctl_table[dir_mode]);
}

void DrvSt7789SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    /* 指定X方向操作区域 */
    DrvSt7789Send(E_SEND_CMD,0x2a);
    DrvSt7789SendU16(x1);
    DrvSt7789SendU16(x2);

    /* 指定Y方向操作区域 */
    DrvSt7789Send(E_SEND_CMD,0x2b);
    DrvSt7789SendU16(y1);
    DrvSt7789SendU16(y2);

    /* 发送该命令，LCD开始等待接收显存数据 */
    DrvSt7789Send(E_SEND_CMD,0x2C);
}



void DrvSt7789Clear(uint16_t color)
{
    #define MAX_SEND 512
    uint32_t i;
    uint32_t total_pixels = (uint32_t)LCD_W * LCD_H;
    uint8_t high = (uint8_t)(color >> 8);
    uint8_t low  = (uint8_t)(color & 0xFF);
    uint8_t buf[MAX_SEND];
    uint16_t pixels_per_buf = sizeof(buf) / 2;

    for (i = 0; i < pixels_per_buf; i++)
    {
        buf[2 * i]     = high;
        buf[2 * i + 1] = low;
    }

    DrvSt7789SetAddress(0, 0, LCD_W - 1, LCD_H - 1);

    LCD_DC(1);
    LCD_CS(0);

    while (total_pixels > 0)
    {
        uint16_t send_pixels = (total_pixels > pixels_per_buf) ? pixels_per_buf : (uint16_t)total_pixels;
        HAL_SPI_Transmit(&hspi2, buf, send_pixels * 2, 1000);
        // DrvSpiSendData(buf, send_pixels * 2);
        total_pixels -= send_pixels;
    }

    LCD_CS(1);
}


void DrvSt7789DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= LCD_W || y >= LCD_H)
    {
        return;
    }

    DrvSt7789SetAddress(x, y, x, y);
    DrvSt7789SendU16(color);
}


void DrvSt7789ShowChar(uint16_t x,
                       uint16_t y,
                       char ch,
                       eAsciiFontDef font,
                       uint16_t fc,
                       uint16_t bc)
{
    uint8_t row, col;
    uint8_t index;

    if (ch < ' ' || ch > '~')
    {
        ch = '?';
    }

    index = (uint8_t)(ch - ' ');

    if (font == E_FONT_1206)
    {
        uint8_t temp;

        for (row = 0; row < 12; row++)
        {
            temp = ascii_1206[index][row];

            for (col = 0; col < 6; col++)
            {
                // if (temp & (0x20 >> col))
                if (temp & (1 << col))
                {
                    DrvSt7789DrawPixel(x + col, y + row, fc);
                }
                else
                {
                    DrvSt7789DrawPixel(x + col, y + row, bc);
                }
            }
        }
    }
    else if (font == E_FONT_2412)
    {
        uint16_t temp;

        for (row = 0; row < 24; row++)
        {
            temp = ((uint16_t)ascii_2412[index][row * 2 + 1] << 8) |
                    (uint16_t)ascii_2412[index][row * 2];

            for (col = 0; col < 12; col++)
            {
                // if (temp & (0x800 >> col))
                if (temp & (0x001 << col))

                {
                    DrvSt7789DrawPixel(x + col, y + row, fc);
                }
                else
                {
                    DrvSt7789DrawPixel(x + col, y + row, bc);
                }
            }
        }
    }
}
void DrvSt7789ShowString(uint16_t x,
                         uint16_t y,
                         const char *str,
                         eAsciiFontDef font,
                         uint16_t fc,
                         uint16_t bc)
{
    uint8_t char_w;
    uint8_t char_h;

    if (str == NULL)
    {
        return;
    }

    if (font == E_FONT_1206)
    {
        char_w = 6;
        char_h = 12;
    }
    else
    {
        char_w = 12;
        char_h = 24;
    }

    while (*str != '\0')
    {
        if (x + char_w > LCD_W)
        {
            x = 0;
            y += char_h;
        }

        if (y + char_h > LCD_H)
        {
            break;
        }

        DrvSt7789ShowChar(x, y, *str, font, fc, bc);
        x += char_w;
        str++;
    }
}


void DrvSt7789FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    #define MAX_SEND 1024

    uint32_t i;
    uint32_t total_pixels;
    uint8_t high;
    uint8_t low;
    uint8_t buf[MAX_SEND];
    uint16_t pixels_per_buf;

    if (x >= LCD_W || y >= LCD_H)
    {
        return;
    }

    if ((x + w) > LCD_W)
    {
        w = LCD_W - x;
    }

    if ((y + h) > LCD_H)
    {
        h = LCD_H - y;
    }

    if (w == 0 || h == 0)
    {
        return;
    }

    total_pixels = (uint32_t)w * h;
    high = (uint8_t)(color >> 8);
    low  = (uint8_t)(color & 0xFF);

    pixels_per_buf = sizeof(buf) / 2;

    for (i = 0; i < pixels_per_buf; i++)
    {
        buf[2 * i]     = high;
        buf[2 * i + 1] = low;
    }

    DrvSt7789SetAddress(x, y, x + w - 1, y + h - 1);

    LCD_DC(1);
    LCD_CS(0);

    while (total_pixels > 0)
    {
        uint16_t send_pixels = (total_pixels > pixels_per_buf) ? pixels_per_buf : (uint16_t)total_pixels;
        HAL_SPI_Transmit(&hspi2, buf, send_pixels * 2, 1000);
        total_pixels -= send_pixels;
    }

    LCD_CS(1);
}



void DrvSt7789FillLvgl(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lv_color_t *color)
{
    uint32_t size;

    if (x1 >= LCD_W || y1 >= LCD_H || x2 >= LCD_W || y2 >= LCD_H)
    {
        return;
    }

    if (x1 > x2 || y1 > y2)
    {
        return;
    }

    size = (uint32_t)(x2 - x1 + 1) * (y2 - y1 + 1);

    DrvSt7789SetAddress(x1, y1, x2, y2);

    LCD_DC(1);
    LCD_CS(0);

    HAL_SPI_Transmit(&hspi2, (uint8_t *)color, size * 2, 1000);

    LCD_CS(1);
}


// void DrvSt7789FillLvgl(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lv_color_t *color)
// {
//     uint32_t size = (x2 - x1 + 1) * (y2 - y1 + 1);

//     DrvSt7789SetAddress(x1, y1, x2, y2);

//     LCD_DC(1);
//     LCD_CS(0);

//     for(uint32_t i = 0; i < size; i++)
//     {
//         uint16_t c = color[i].full;

//         // 高低字节交换
//         c = (c << 8) | (c >> 8);

//         HAL_SPI_Transmit(&hspi2, (uint8_t *)&c, 2, HAL_MAX_DELAY);
//     }

//     LCD_CS(1);
// }

// void DrvSt7789FillLvgl(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lv_color_t *color)
// {
//     uint16_t width  = (x2 - x1) + 1;
//     uint16_t height = (y2 - y1) + 1;

//     static uint8_t line_buf[240 * 2];

//     DrvSt7789SetAddress(x1, y1, x2, y2);

//     LCD_DC(1);
//     LCD_CS(0);

//     for(uint16_t row = 0; row < height; row++)
//     {
//         for(uint16_t col = 0; col < width; col++)
//         {
//             uint16_t c = color[row * width + col].full;
//             line_buf[2 * col]     = (uint8_t)(c >> 8);
//             line_buf[2 * col + 1] = (uint8_t)(c & 0xFF);
//         }

//         HAL_SPI_Transmit(&hspi2, line_buf, width * 2, HAL_MAX_DELAY);
//     }

//     LCD_CS(1);
// }

void Drvst7789Init(void)
{
    // 背光控制
    LCD_BLK(1);
    // 芯片选择和复位引脚初始化
    LCD_CS(1);
    LCD_RST(1);
    HAL_Delay(20);
    LCD_RST(0);
    HAL_Delay(120);
    LCD_RST(1);
    HAL_Delay(120);

    // 关闭睡眠模式
    DrvSt7789Send(E_SEND_CMD,0x11);
    HAL_Delay(150);
    DrvSt7789SetDir(USE_HORIZONTAL);
#if DEVICE_IC == 1

    /* 开始设置显存扫描模式，数据格式等 */
    DrvSt7789Send(E_SEND_CMD,0x36);
    DrvSt7789Send(E_SEND_DATA,0x00);
    /* RGB 5-6-5-bit格式  */
    DrvSt7789Send(E_SEND_CMD,0x3A);
    DrvSt7789Send(E_SEND_DATA,0x05);
    /* porch 设置 */
    DrvSt7789Send(E_SEND_CMD,0xB2);
    DrvSt7789Send(E_SEND_DATA,0x0C);
    DrvSt7789Send(E_SEND_DATA,0x0C);
    DrvSt7789Send(E_SEND_DATA,0x00);
    DrvSt7789Send(E_SEND_DATA,0x33);
    DrvSt7789Send(E_SEND_DATA,0x33);
    /* VGH设置 */
    DrvSt7789Send(E_SEND_CMD,0xB7);
    DrvSt7789Send(E_SEND_DATA,0x46);
    /* VCOM 设置 */
    DrvSt7789Send(E_SEND_CMD,0xBB);
    DrvSt7789Send(E_SEND_DATA,0x1B);
    /* LCM 设置 */
    DrvSt7789Send(E_SEND_CMD,0xC0);
    DrvSt7789Send(E_SEND_DATA,0x2C);
    /* VDV and VRH 设置 */
    DrvSt7789Send(E_SEND_CMD,0xC2);
    DrvSt7789Send(E_SEND_DATA,0x01);
    /* VRH 设置 */
    DrvSt7789Send(E_SEND_CMD,0xC3);
    DrvSt7789Send(E_SEND_DATA,0x0F);
    /* VDV 设置 */
    DrvSt7789Send(E_SEND_CMD,0xC4);
    DrvSt7789Send(E_SEND_DATA,0x20);
    /* 普通模式下显存速率设置 60Mhz */
    DrvSt7789Send(E_SEND_CMD,0xC6);
    DrvSt7789Send(E_SEND_DATA,0x0F);
    /* 电源控制 */
    DrvSt7789Send(E_SEND_CMD,0xD0);
    DrvSt7789Send(E_SEND_DATA,0xA4);
    DrvSt7789Send(E_SEND_DATA,0xA1);

    DrvSt7789Send(E_SEND_CMD,0xD6);
    DrvSt7789Send(E_SEND_DATA,0xA1);

    /* 电压设置 */
    DrvSt7789Send(E_SEND_CMD,0xE0);


    DrvSt7789Send(E_SEND_DATA,0xF0);
    DrvSt7789Send(E_SEND_DATA,0x00);
    DrvSt7789Send(E_SEND_DATA,0x06);
    DrvSt7789Send(E_SEND_DATA,0x04);
    DrvSt7789Send(E_SEND_DATA,0x05);
    DrvSt7789Send(E_SEND_DATA,0x05);
    DrvSt7789Send(E_SEND_DATA,0x31);
    DrvSt7789Send(E_SEND_DATA,0x44);
    DrvSt7789Send(E_SEND_DATA,0x48);
    DrvSt7789Send(E_SEND_DATA,0x36);
    DrvSt7789Send(E_SEND_DATA,0x12);
    DrvSt7789Send(E_SEND_DATA,0x12);
    DrvSt7789Send(E_SEND_DATA,0x2B);
    DrvSt7789Send(E_SEND_DATA,0x34);

        /* 电压设置 */
    DrvSt7789Send(E_SEND_CMD,0xE1);
    DrvSt7789Send(E_SEND_DATA,0xE1);
    DrvSt7789Send(E_SEND_DATA, 0xF0);
    DrvSt7789Send(E_SEND_DATA, 0x0B);
    DrvSt7789Send(E_SEND_DATA, 0x0F);
    DrvSt7789Send(E_SEND_DATA, 0x0F);
    DrvSt7789Send(E_SEND_DATA, 0x0D);
    DrvSt7789Send(E_SEND_DATA, 0x26);
    DrvSt7789Send(E_SEND_DATA, 0x31);
    DrvSt7789Send(E_SEND_DATA, 0x43);
    DrvSt7789Send(E_SEND_DATA, 0x47);
    DrvSt7789Send(E_SEND_DATA, 0x38);
    DrvSt7789Send(E_SEND_DATA, 0x14);
    DrvSt7789Send(E_SEND_DATA, 0x14);
    DrvSt7789Send(E_SEND_DATA, 0x2C);
    DrvSt7789Send(E_SEND_DATA, 0x32);
    /* 显示开 */
    DrvSt7789Send(E_SEND_CMD,0x21);
    DrvSt7789Send(E_SEND_CMD,0x29);
    // DrvSt7789Send(E_SEND_CMD,0x2C);
#elif DEVICE_IC == 0

    DrvSt7789Send(E_SEND_CMD,0x3a);
    DrvSt7789Send(E_SEND_DATA,0x05); // 
    //--------------------------------ST7789V Frame rate setting-----------------

    DrvSt7789Send(E_SEND_CMD,0xb2);
    DrvSt7789Send(E_SEND_DATA,0x0c);
    DrvSt7789Send(E_SEND_DATA,0x0c);
    DrvSt7789Send(E_SEND_DATA,0x00);
    DrvSt7789Send(E_SEND_DATA,0x33);
    DrvSt7789Send(E_SEND_DATA,0x33);
    DrvSt7789Send(E_SEND_CMD,0xb7);
    DrvSt7789Send(E_SEND_DATA,0x35);
    //---------------------------------ST7789V Power setting---------------------

    DrvSt7789Send(E_SEND_CMD,0xbb);
    DrvSt7789Send(E_SEND_DATA,0x19); // 0x28
    DrvSt7789Send(E_SEND_CMD,0xc0);
    DrvSt7789Send(E_SEND_DATA,0x2c);
    DrvSt7789Send(E_SEND_CMD,0xc2);
    DrvSt7789Send(E_SEND_DATA,0x01);
    DrvSt7789Send(E_SEND_CMD,0xc3);
    DrvSt7789Send(E_SEND_DATA,0x12); // 0x10
    DrvSt7789Send(E_SEND_CMD,0xc4);
    DrvSt7789Send(E_SEND_DATA,0x20);
    DrvSt7789Send(E_SEND_CMD,0xc6);
    DrvSt7789Send(E_SEND_DATA,0x0f);
    DrvSt7789Send(E_SEND_CMD,0xd0);
    DrvSt7789Send(E_SEND_DATA,0xa4);
    DrvSt7789Send(E_SEND_DATA,0xa1);
    //--------------------------------ST7789V gamma setting----------------------

    DrvSt7789Send(E_SEND_CMD,0xe0);
    DrvSt7789Send(E_SEND_DATA,0xd0);
    DrvSt7789Send(E_SEND_DATA,0x04); // 0x00
    DrvSt7789Send(E_SEND_DATA,0x0d); // 0x02
    DrvSt7789Send(E_SEND_DATA,0x11); // 0x07
    DrvSt7789Send(E_SEND_DATA,0x13); // 0x0a
    DrvSt7789Send(E_SEND_DATA,0x2b); // 0x28
    DrvSt7789Send(E_SEND_DATA,0x3f); // 0x32
    DrvSt7789Send(E_SEND_DATA,0x54); // 0x44
    DrvSt7789Send(E_SEND_DATA,0x4c); // 0x42
    DrvSt7789Send(E_SEND_DATA,0x18); // 0x06
    DrvSt7789Send(E_SEND_DATA,0x0d); // 0x0e
    DrvSt7789Send(E_SEND_DATA,0x0b); // 0x12
    DrvSt7789Send(E_SEND_DATA,0x1f); // 0x14
    DrvSt7789Send(E_SEND_DATA,0x23); // 0x17
    DrvSt7789Send(E_SEND_CMD,0xe1);
    DrvSt7789Send(E_SEND_DATA,0xd0); // 0xd0
    DrvSt7789Send(E_SEND_DATA,0x04); // 0x00
    DrvSt7789Send(E_SEND_DATA,0x0c); // 0x02
    DrvSt7789Send(E_SEND_DATA,0x11); // 0x07
    DrvSt7789Send(E_SEND_DATA,0x13); // 0x0a
    DrvSt7789Send(E_SEND_DATA,0x2c); // 0x28
    DrvSt7789Send(E_SEND_DATA,0x3f); // 0x31
    DrvSt7789Send(E_SEND_DATA,0x44); // 0x54
    DrvSt7789Send(E_SEND_DATA,0x51); // 0x47
    DrvSt7789Send(E_SEND_DATA,0x2f); // 0x0e
    DrvSt7789Send(E_SEND_DATA,0x1f); // 0x1c
    DrvSt7789Send(E_SEND_DATA,0x1f); // 0x17
    DrvSt7789Send(E_SEND_DATA,0x20); // 0x1b
    DrvSt7789Send(E_SEND_DATA,0x23); // 0x1e

    DrvSt7789Send(E_SEND_CMD,0x20);

    DrvSt7789Send(E_SEND_CMD,0x29);
#endif
    DrvSt7789Clear(WHITE);
    DrvSt7789ShowString(0,0,"HELLO1111",E_FONT_1206,BLACK,WHITE);
    DrvSt7789ShowString(50,50,"HELLO2222",E_FONT_2412,WHITE,BLACK);
    // DrvSt7789Clear(WHITE);
    // DrvSt7789Clear(LBBLUE);
    // DrvSt7789FillRect(20, 30, 100, 50, 0xF800);
}