/**
  ******************************************************************************
  * @file           : keypad16.h
  * @brief          : 4x4 Keypad driver header file.
  * @author         : Gemini
  * @date           : 2025-09-23
  ******************************************************************************
  * @attention
  *
  * This driver implements scanning for a 4x4 matrix keypad.
  * It provides functions to detect both key holds and single-press triggers.
  *
  * GPIO Configuration (in STM32CubeMX):
  * Rows (R1-R4) -> PC0, PC1, PC2, PC3 : GPIO_Output, Push-Pull, No pull-up/down
  * Columns (C1-C4) -> PC6, PC7, PC8, PC9 : GPIO_Input, Pull-up
  *
  ******************************************************************************
  */

#ifndef INC_KEYPAD16_H_
#define INC_KEYPAD16_H_

/* C/C++ HAL 라이브러리 및 표준 라이브러리 포함 */
#include "main.h" // STM32CubeMX가 생성하는 메인 헤더 파일

/**
 * @brief 키가 눌리지 않았을 때 반환되는 값입니다.
 */
#define NO_KEY_PRESSED '\0'

/* 키패드 핀 및 포트 정의 */
// 행(Row) 핀 설정 (출력)
#define R1_PORT GPIOC
#define R1_PIN  GPIO_PIN_0
#define R2_PORT GPIOC
#define R2_PIN  GPIO_PIN_1
#define R3_PORT GPIOC
#define R3_PIN  GPIO_PIN_2
#define R4_PORT GPIOC
#define R4_PIN  GPIO_PIN_3

// 열(Column) 핀 설정 (입력)
#define C1_PORT GPIOC
#define C1_PIN  GPIO_PIN_6
#define C2_PORT GPIOC
#define C2_PIN  GPIO_PIN_7
#define C3_PORT GPIOC
#define C3_PIN  GPIO_PIN_8
#define C4_PORT GPIOC
#define C4_PIN  GPIO_PIN_9


/**
 * @brief 키패드 드라이버를 초기화합니다.
 * @note  이 함수는 애플리케이션 시작 시 한 번만 호출해야 합니다.
 *        주로 키 상태 변수를 초기화하는 역할을 합니다.
 */
void KEYPAD16_Init(void);

/**
 * @brief 키패드 매트릭스를 스캔하여 현재 눌린 키를 감지합니다.
 * @note  이 함수는 main 함수의 while(1) 루프 안에서 주기적으로 호출되어야 합니다.
 *        약 10-20ms 간격으로 호출하는 것이 이상적입니다. (HAL_Delay 사용 가능)
 *        이 함수가 호출될 때마다 내부 키 상태가 업데이트됩니다.
 */
void KEYPAD16_Scan(void);

/**
 * @brief 현재 '계속 누르고 있는' 키의 문자를 반환합니다.
 * @note  키에서 손을 떼면 NO_KEY_PRESSED를 반환합니다.
 *        여러 키가 눌리면 스캔 순서상 먼저 감지된 키 하나만 반환합니다.
 * @param None
 * @retval char 눌린 키의 문자. 눌린 키가 없으면 NO_KEY_PRESSED ('\0').
 */
char KEYPAD16_Get_Pressed_Key(void);

/**
 * @brief 키가 '처음 눌리는 순간'에만 한 번 키의 문자를 반환합니다 (Rising Edge 감지).
 * @note  키를 계속 누르고 있어도 두 번째 호출부터는 NO_KEY_PRESSED를 반환합니다.
 *        키에서 손을 뗐다가 다시 눌러야 해당 키의 문자가 다시 반환됩니다.
 * @param None
 * @retval char 새로 눌린 키의 문자. 새로 눌린 키가 없으면 NO_KEY_PRESSED ('\0').
 */
char KEYPAD16_Get_Triggered_Key(void);


#endif /* INC_KEYPAD16_H_ */