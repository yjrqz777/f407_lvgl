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



void timeCallBackTask(void);
void timeCallBackTask(void)
{
	//闪烁LED1
	lv_tick_inc(1);
//	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,1);
}


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
//    rect_dsc.bg_opa = LV_OPA_COVER;
//    rect_dsc.bg_grad.dir = LV_GRAD_DIR_HOR;
//    rect_dsc.bg_grad.stops[0].color = lv_palette_main(LV_PALETTE_RED);
//    rect_dsc.bg_grad.stops[1].color = lv_palette_main(LV_PALETTE_BLUE);
//    rect_dsc.border_width = 2;
//    rect_dsc.border_opa = LV_OPA_90;
//    rect_dsc.border_color = lv_color_white();
//    rect_dsc.shadow_width = 5;
//    rect_dsc.shadow_ofs_x = 5;
//    rect_dsc.shadow_ofs_y = 5;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_palette_main(LV_PALETTE_ORANGE);

    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];

    lv_obj_t * canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    lv_obj_center(canvas);
    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_OPA_COVER);

    lv_canvas_draw_rect(canvas, 70, 60, 100, 70, &rect_dsc);

    lv_canvas_draw_text(canvas, 40, 20, 100, &label_dsc, "Some text on text canvas");

    /*Test the rotation. It requires another buffer where the original image is stored.
     *So copy the current image to buffer and rotate it to the canvas*/
//    static lv_color_t cbuf_tmp[CANVAS_WIDTH * CANVAS_HEIGHT];
//    memcpy(cbuf_tmp, cbuf, sizeof(cbuf_tmp));
//    lv_img_dsc_t img;
//    img.data = (void *)cbuf_tmp;
//    img.header.cf = LV_IMG_CF_TRUE_COLOR;
//    img.header.w = CANVAS_WIDTH;
//    img.header.h = CANVAS_HEIGHT;

//    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_OPA_COVER);
//    lv_canvas_transform(canvas, &img, 120, LV_IMG_ZOOM_NONE, 0, 0, CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, true);

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
   lv_obj_set_style_radius(obj,LV_RADIUS_CIRCLE,0);   //设置样式圆角

   lv_obj_align(obj,LV_ALIGN_LEFT_MID,10,0);          //居中样式

   lv_anim_t a;                                       //创建动画样式
   lv_anim_init(&a);                                  //初始化动画
   lv_anim_set_var(&a,obj);                           //给动画设置一个变量
   lv_anim_set_values(&a,10,50);                      //设置一个动画值
   lv_anim_set_time(&a,1000);                         //设置动画时间
   lv_anim_set_playback_delay(&a,100);                //回放延时 使动画回放时，正向方向准备好了
   lv_anim_set_playback_time(&a,1000);                 //回放时间
   lv_anim_set_repeat_delay(&a,500);                  //重复延时
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
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim4);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
//	
lv_init();

lv_port_disp_init();

HAL_Delay(500);
//lv_example_canvas_2();

//lv_obj_t *obj = lv_checkbox_create(lv_scr_act());
////lv_obj_set_size(obj,40,10);
//lv_obj_align(obj,LV_ALIGN_CENTER,0, 0);
////lv_timer_handler();
////HAL_Delay(5000);
//lv_obj_t *obj2 = lv_checkbox_create(lv_scr_act());
//lv_obj_align(obj2,LV_ALIGN_CENTER,40,60);


//    static lv_style_t style;
//    lv_style_init(&style);
//    lv_style_set_radius(&style, 5);

//    /*Make a gradient*/
//    lv_style_set_bg_opa(&style, LV_OPA_COVER);
//    lv_grad_dsc_t grad;
//    grad.dir = LV_GRAD_DIR_VER;
//    grad.stops_count = 2;
//    grad.stops[0].color = lv_palette_lighten(LV_PALETTE_GREY, 1);
//    grad.stops[1].color = lv_palette_main(LV_PALETTE_BLUE);

//    /*Shift the gradient to the bottom*/
//    grad.stops[0].frac  = 128;
//    grad.stops[1].frac  = 192;

//    lv_style_set_bg_grad(&style, &grad);

//    /*Create an object with the new style*/
//    lv_obj_t * obj = lv_obj_create(lv_scr_act());
//    lv_obj_add_style(obj, &style, 0);
//    lv_obj_center(obj);


    LV_IMG_DECLARE(emoji_F617);
    lv_obj_t * img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, &emoji_F617);
    lv_obj_set_pos(img, 100, 100);//设置图片位置
    lv_img_set_angle(img, 0);//设置图片旋转角
    lv_img_set_zoom(img,500);//设置图片缩放

//anim_show_2();
//	lv_example_flex_5();
	
//	st7789v_init();
	
//	LCD_color_fill_lvgl(2, 20, 30, 40, 0xf800);
//	LCD_color_fill_lvgl(30, 40, 70, 80, 0xf800);
  while (1)
  {
//		printf("1111\n");
//		printf("%d\n",ii);
		
		lv_timer_handler();
		HAL_Delay(5);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
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
