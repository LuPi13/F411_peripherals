/*
 * lcd1602.h
 *
 *  Created on: Sep 29, 2025
 *      Author: UOS
 */

#ifndef INC_LCD1602_H_
#define INC_LCD1602_H_


#include "stm32f4xx_hal.h"  // MCU에 맞게 변경

// I²C 핸들 (main.c 등에서 extern 선언한 걸 받아옴)
extern I2C_HandleTypeDef hi2c1;

// LCD I²C 주소 (7비트)
#define LCD_ADDR (0x3C)  // 0x78 >> 1

void LCD_Init(void);
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_DispCGRAM(void);
void LCD_DispChar(int line, int column, char *dp);

#endif
