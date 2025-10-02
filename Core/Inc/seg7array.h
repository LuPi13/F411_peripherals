
#ifndef INC_SEG7ARRAY_H_
#define INC_SEG7ARRAY_H_

#include "seg7array.h"
#include "stm32f4xx_hal.h"
#include "main.h"

/**
 * @brief 초기화 함수
 */
void SEG7ARRAY_Init(void);

/**
 * @brief 입력받은 position의 cathode 비트를 수정
 * @note 1이 불켜짐, 0이 불꺼짐
 */
void SEG7ARRAY_Set_cathode(uint8_t pos, uint8_t cathode_bits);

/**
 * @brief 자리를 순회하며 불빛을 켬
 * @note while문 같이 빠르게 반복되는 곳에 입력 필요
 */
void SEG7ARRAY_Cycle(void);


#endif /* INC_SEG7ARRAY_H_ */
