/*
 * usart2console.h
 *
 *  Created on: Sep 5, 2025
 *      Author: UOS
 */

#ifndef INC_USART2CONSOLE_H_
#define INC_USART2CONSOLE_H_

#include "stm32f4xx_hal.h"   // MCU에 맞게 변경 필요
#include <stdint.h>
#include <stdbool.h>

extern UART_HandleTypeDef huart2;
#define U2C_USART_CHANNEL &huart2

#define U2C_RX_BUFFER_SIZE 64   // 입력 버퍼 크기
#define U2C_TX_BUFFER_SIZE 128  // 출력 버퍼 크기
#define U2C_TX_TIMEOUT_MS 10    // 전송 타임아웃 (ms), U2C_print 함수가 이 시간동안 전송을 재시도


// 줄바꿈 옵션
#define NEWLINE_USE_CRLF

#ifdef NEWLINE_USE_CRLF
    #define NEWLINE_CHARACTER "\r\n"
#endif
#ifdef NEWLINE_USE_LF
    #define NEWLINE_CHARACTER "\n"
#endif
#ifdef NEWLINE_USE_CR
    #define NEWLINE_CHARACTER "\r"
#endif

// 엔터키 옵션
#define ENTER_USE_CR

#ifdef ENTER_USE_CRLF
    #define ENTER_CHARACTER "\r\n"
#endif
#ifdef ENTER_USE_LF
    #define ENTER_CHARACTER "\n"
#endif
#ifdef ENTER_USE_CR
    #define ENTER_CHARACTER "\r"
#endif

void U2C_init(void);
HAL_StatusTypeDef U2C_print(uint8_t *pString, uint16_t length);
HAL_StatusTypeDef U2C_println(uint8_t *pString, uint16_t length);

void U2C_RxCpltCallback(void);  // HAL_UART_RxCpltCallback에서 호출 필요
void U2C_process(void);         // main 루프에서 주기적으로 호출

void U2C_TxCpltCallback(void);  // HAL_UART_TxCpltCallback에서 호출 필요


#endif /* INC_USART2CONSOLE_H_ */
