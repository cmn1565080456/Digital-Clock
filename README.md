# Digital-Clock
Porject of designing digital based on STM32F411RE&amp;Nucleo
项目简介：基于stm32f411re芯片与nucleo64开发板实现数字时钟
项目功能：1.基于RTC实现定时器与时钟以及日历；
         2.设计GUI界面
         3.基于ssd1306驱动芯片将7pin，0.96inchOLED点亮并实时显示GUI界面
         4.基于LM75A测温模块测量环境温度
软件平台：MDK-ARM keil5与STM32CUBEMX仿真平台

附加功能：OLED显示中文字符，此功能需要额外的取模软件将汉字转码实现，相关的工具见TOOLS文件夹
