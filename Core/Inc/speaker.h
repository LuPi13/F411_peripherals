
#ifndef INC_SPEAKER_H_
#define INC_SPEAKER_H_

#include "main.h"
#include "stm32f4xx_hal.h" // 자신의 MCU에 맞는 HAL 드라이버 헤더로 변경하세요.

/**
 * @brief  스피커 모듈을 초기화하고 타이머를 지정합니다.
 * @param  htim: 스피커 제어에 사용할 타이머의 핸들러
 */
void SPEAKER_Init(TIM_HandleTypeDef *htim);

/**
 * @brief  지정된 주파수로 지정된 시간만큼 스피커 재생을 시작합니다. (논블로킹)
 * @param  frequency: 재생할 주파수 (Hz)
 * @param  play_time_ms: 재생할 시간 (milliseconds)
 */
void SPEAKER_Start(uint32_t frequency, uint32_t play_time_ms);

/**
 * @brief 스피커 재생을 즉시 중지합니다.
 */
void SPEAKER_Stop(void);

/**
 * @brief  타이머 콜백 함수에서 주기적으로 호출될 함수. 실제 재생 로직을 처리합니다.
 */
void SPEAKER_Loop(void);

/**
 * @brief  스피커가 현재 재생 중인지 상태를 반환합니다.
 * @retval 0: 정지, 1: 재생 중
 */
uint8_t SPEAKER_IsPlaying(void);


#endif /* INC_SPEAKER_H_ */
