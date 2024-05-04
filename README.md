# stm32f407-hal-dma-lvgl8.3-st7789v-ft6336-320*280-freertos

| 硬件      | 使能 | 软件     | 使能 |
|-----------|------|----------|------|
| stm32f407 | ✅    | hal      | ✅    |
| st7789v   | ✅    | dma      | ✅    |
| ft6336    | ✅    | lvgl8.3  | ✅    |
| 320*280   | ✅    | freertos | ✅    |

## st7789v引脚      & ft6336引脚

| st7789v | f407 | 说明 |  | ft6336 | f407 | 说明       |
|---------|------|------|--|--------|------|------------|
| SDA     | PA7  |      |  | SDA    | PB11 |            |
| SCL     | PA5  |      |  | SCL    | PB10 |            |
| RES     | PA4  |      |  | INT    | NULL | 暂时未使用 |
| DC      | PA3  |      |  | RET    | NULL | 暂时未使用 |
| CS      | PA2  |      |  |        |      |            |
| BL      | PA0  | pwm  |  |        |      |            |



<!-- 对勾：✅

    叉号：❌ -->

* [x] ReadMe
<!-- - [ ] ReadMe_CN -->

2024年5月3日18:57:34 增加触摸 ft6336 ~~暂时没使用dma i2c~~

2024年5月4日20:24:09 移植入freertos

 [屏幕链接](https://m.tb.cn/h.g1DUCAdmPUBtx31?tk=lYC6WHEW9lO "「2.8寸TFT液晶显示屏SPI串口ST7789V彩屏240*320电容触摸屏幕lcd屏」")。

<!-- 【淘宝】https://m.tb.cn/h.g1DUCAdmPUBtx31?tk=lYC6WHEW9lO ZH4920 「2.8寸TFT液晶显示屏SPI串口ST7789V彩屏240*320电容触摸屏幕lcd屏」 -->

<video src="https://github.com/yjrqz777/f407_lvgl/assets/76079782/13095bca-dbe2-4a00-bcfc-02d91b443fa8" controls="controls" width="500" height="300">您的浏览器不支持播放该视频！</video>

<!-- https://github.com/yjrqz777/f407_lvgl/assets/76079782/13095bca-dbe2-4a00-bcfc-02d91b443fa8 -->
