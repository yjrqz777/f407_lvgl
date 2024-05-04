/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stm32f4xx_it.h"
#include "st7789v.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "ft6336.h"


#define blk_value(n) n<100?n=100:n=n;n>1000?n=1000:n=n

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
int fputc(int ch, FILE *f){
 uint8_t temp[1] = {ch};
 HAL_UART_Transmit(&huart6, temp, 1, 0xffff);
return ch;
}

int fgetc(FILE * f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart6,&ch, 1, 0xffff);
  return ch;
}
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */



//void timeCallBackTask(void);
//void timeCallBackTask(void)
//{
//	//闪烁LED1
//	lv_tick_inc(1);
////	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,1);
//}


#define CANVAS_WIDTH  100
#define CANVAS_HEIGHT  150

/**
 * Create a transparent canvas with Chroma keying and indexed color format (palette).
 */
void lv_example_canvas_2(void)
{
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.radius = 10;
    rect_dsc.bg_opa = LV_OPA_COVER;
    rect_dsc.bg_grad.dir = LV_GRAD_DIR_HOR;
    rect_dsc.bg_grad.stops[0].color = lv_palette_main(LV_PALETTE_RED);
    rect_dsc.bg_grad.stops[1].color = lv_palette_main(LV_PALETTE_BLUE);
    rect_dsc.border_width = 2;
    rect_dsc.border_opa = LV_OPA_90;
    rect_dsc.border_color = lv_color_white();
    rect_dsc.shadow_width = 5;
    rect_dsc.shadow_ofs_x = 5;
    rect_dsc.shadow_ofs_y = 5;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_palette_main(LV_PALETTE_ORANGE);

    lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];

    lv_obj_t * canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    lv_obj_center(canvas);
    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_OPA_COVER);

    lv_canvas_draw_rect(canvas, 70, 60, 100, 70, &rect_dsc);

    lv_canvas_draw_text(canvas, 40, 20, 100, &label_dsc, "Some text on text canvas");

    /*Test the rotation. It requires another buffer where the original image is stored.
     *So copy the current image to buffer and rotate it to the canvas*/
    lv_color_t cbuf_tmp[CANVAS_WIDTH * CANVAS_HEIGHT];
    memcpy(cbuf_tmp, cbuf, sizeof(cbuf_tmp));
    lv_img_dsc_t img;
    img.data = (void *)cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = CANVAS_WIDTH;
    img.header.h = CANVAS_HEIGHT;

    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_OPA_COVER);
    lv_canvas_transform(canvas, &img, 120, LV_IMG_ZOOM_NONE, 0, 0, CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, true);


}


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


static void anim_x_cb(void *var,int32_t v)
{
   lv_obj_set_x(var,v);
}
/*************************************************
 *  函数名称 :  anim_size_cb
 *  参    数 ： 无
 *  函数功能 ： 动画尺寸显示回调函数
 *************************************************/
static void anim_size_cb(void *var,int32_t v)
{
   lv_obj_set_size(var,v,v);
}
/*************************************************
 *  函数名称 :  anim_show_2
 *  参    数 ： 无
 *  函数功能 ： 按键联动，实现Label动画显示
 *************************************************/
void anim_show_2()
{
   lv_obj_t * obj = lv_obj_create(lv_scr_act());      //创建一个对象
   lv_obj_set_style_bg_color(obj,lv_palette_main(LV_PALETTE_RED),0); //设置背景颜色
   lv_obj_set_style_radius(obj,0xFFF,0);   //设置样式圆角
   lv_obj_align(obj,LV_ALIGN_TOP_LEFT,10,0);          //居中样式

   lv_anim_t a;                                       //创建动画样式
   lv_anim_init(&a);                                  //初始化动画
   lv_anim_set_var(&a,obj);                           //给动画设置一个变量
   lv_anim_set_values(&a,5,70);                      //设置一个动画值
   lv_anim_set_time(&a,1000);                         //设置动画时间
   lv_anim_set_playback_delay(&a,300);                //回放延时 使动画回放时，正向方向准备好了
   lv_anim_set_playback_time(&a,1000);                 //回放时间
   lv_anim_set_repeat_delay(&a,5);                  //重复延时
   lv_anim_set_repeat_count(&a,LV_ANIM_REPEAT_INFINITE); //重复计数次数
   lv_anim_set_path_cb(&a,lv_anim_path_ease_in_out);  //设置动画播放路径

   lv_anim_set_exec_cb(&a,anim_size_cb);              //给动画设置一个功能 回调函数为尺寸
   lv_anim_start(&a);                                 //开始动画
   lv_anim_set_exec_cb(&a,anim_x_cb);                 //给动画设置一个功能 回调函数为x轴值
   lv_anim_set_values(&a,10,240);                     //给动画设置一个值
   lv_anim_start(&a);                                 //开始动画
}

void lv_example_style_2(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, 5);

    /*Make a gradient*/
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 2;
    grad.stops[0].color = lv_palette_lighten(LV_PALETTE_GREY, 1);
    grad.stops[1].color = lv_palette_main(LV_PALETTE_BLUE);

    /*Shift the gradient to the bottom*/
    grad.stops[0].frac  = 128;
    grad.stops[1].frac  = 192;

    lv_style_set_bg_grad(&style, &grad);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj, &style, 0);
    lv_obj_center(obj);
}


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */











static void row_gap_anim(void * obj, int32_t v)
{
    lv_obj_set_style_pad_row(obj, v, 0);
}

static void column_gap_anim(void * obj, int32_t v)
{
    lv_obj_set_style_pad_column(obj, v, 0);
}

/**
 * Demonstrate the effect of column and row gap style properties
 */
void lv_example_flex_5(void)
{
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 300, 220);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);

    uint32_t i;
    for(i = 0; i < 9; i++) {
        lv_obj_t * obj = lv_obj_create(cont);
        lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "%"LV_PRIu32, i);
        lv_obj_center(label);
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, cont);
    lv_anim_set_values(&a, 0, 10);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    lv_anim_set_exec_cb(&a, row_gap_anim);
    lv_anim_set_time(&a, 500);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_start(&a);

    lv_anim_set_exec_cb(&a, column_gap_anim);
    lv_anim_set_time(&a, 3000);
    lv_anim_set_playback_time(&a, 3000);
    lv_anim_start(&a);
}






void lv_example_style_12(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_border_color(&style, lv_palette_lighten(LV_PALETTE_GREEN, 3));
    lv_style_set_border_width(&style, 3);

    lv_obj_t * obj = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj, &style, 0);

    /*Overwrite the background color locally*/
    lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN);

    lv_obj_center(obj);
}





/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//static int do_i2cdetect_cmd()
//{
//    uint8_t address;
//    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");
//    for (int i = 0; i < 128; i += 16) {
//        printf("%02x: ", i);
//        for (int j = 0; j < 16; j++) {
//            fflush(stdout);
//            address = i + j;
//						uint8_t ret =HAL_I2C_IsDeviceReady(&hi2c2, (address << 1) | 1, 2, 500);
//            if (ret == HAL_OK) {
//                printf("%02x ", address);
//            } else if (ret == HAL_TIMEOUT) {
//                printf("UU ");
//            } else {
//                printf("-- ");
//            }
//        }
//        printf("\r\n");
//    }
////		uint8_t data[8]={0};
////		HAL_I2C_Mem_Read(&hi2c2,(0x38 << 1) | 1,0xA5,1,data, 1,1000);
////		for(int i = 0;i<1;i++)
////		printf("-%02X-\r\n",data[i]);
//		
//}



static void slider_event_cb(lv_event_t * e);
static lv_obj_t * slider_label;

/**
 * A default slider with a label displaying the current value
 */
void lv_example_slider_1(void)
{
    /*Create a slider in the center of the display*/
    lv_obj_t * slider = lv_slider_create(lv_scr_act());
    lv_obj_center(slider);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /*Create a label below the slider*/
    slider_label = lv_label_create(lv_scr_act());
    lv_label_set_text(slider_label, "0%");

    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
		uint16_t value = (int)lv_slider_get_value(slider)*10;
    char buf[8];
		uint16_t blk = 300;
    lv_snprintf(buf, sizeof(buf), "%d%%", value/10);
		blk = value<300?300:value;

		htim2.Instance->CCR1 = blk;
    lv_label_set_text(slider_label, buf);
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}







/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USART6_UART_Init();
  MX_TIM4_Init();
  MX_I2C2_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim4);


  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
//	htim2.Instance->CCR1 = 300;
//	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 300);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//	do_i2cdetect_cmd();
	lv_init();

	lv_port_disp_init();
	
	lv_port_indev_init();
	HAL_Delay(500);
//lv_example_canvas_2();


    LV_IMG_DECLARE(emoji_F617);
    lv_obj_t * img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, &emoji_F617);
    lv_obj_set_pos(img, 35, 35);//设置图片位置
    lv_img_set_angle(img, 30);//设置图片旋转角
    lv_img_set_zoom(img,200);//设置图片缩放
	
		
		lv_obj_t * box =lv_checkbox_create(lv_scr_act());
		lv_obj_align(box,LV_ALIGN_BOTTOM_LEFT,0,0);


//		anim_show_2();
//		lv_example_get_started_3();
		
		
//		static lv_point_t line_points[] = { {5, 5}, {70, 70}, {120, 10}};

///* 创建一个 line 组件(对象)，他的父对象是活动屏幕对象 */
//		lv_obj_t * line = lv_line_create(lv_scr_act());

//		lv_line_set_points(line, line_points, 3);     	// 设置点数组。line将连接这些点，按顺序画出直线

		
//lv_example_canvas_2();
//	lv_example_flex_5();

		
//		lv_timer_handler();
		lv_example_slider_1();
		anim_show_2();
		
		    lv_obj_t *parent = lv_obj_create(lv_scr_act());
    lv_obj_set_size(parent,100,100);
    lv_obj_set_style_bg_color(parent,lv_palette_main(LV_PALETTE_BLUE),0);
    lv_obj_align(parent,LV_ALIGN_CENTER,0,0);

    lv_obj_t *child = lv_obj_create(parent);
    lv_obj_set_size(child,50,50);
    lv_obj_set_style_bg_color(child,lv_palette_main(LV_PALETTE_GREEN),0);
    lv_obj_align(child,LV_ALIGN_CENTER,0,0);

//	uint8_t data2= 0x05;
//	uint8_t data23[2];

//	uint16_t DevAddress = 0x38;
//	uint8_t ret = 0;
//	do_i2cdetect_cmd()
//	HAL_I2C_Mem_Write(&hi2c2,(0x38 << 1) | 0,0x87,1,&data2, 1,1000);
//	HAL_I2C_Mem_Read(&hi2c2,(0x38 << 1) | 1,0x87,1,data23, 1,1000);
//	printf("-%02X-\r\n",data23[0]);
//	HAL_Delay	(1000);
//lv_timer_handler();
  while (1)
  {

		
//		lv_tick_inc(1);
//		ft6336_read_xy();
		lv_timer_handler();

		HAL_Delay(10);
		
//	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_2);
		
		
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
