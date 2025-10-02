
#include "speaker.h"

#define SAMPLING_RATE 100000 // 타이머 콜백 주파수 (100kHz)

// ===== 내부 변수 (static으로 선언하여 이 파일 안에서만 접근 가능) =====
// 이 모듈이 사용할 타이머 핸들러 포인터
static TIM_HandleTypeDef *speaker_htim;

// 스피커 재생 상태 플래그
static volatile uint8_t is_playing = 0;

// 주파수 제어용 변수
static volatile uint32_t toggle_period_ticks = 0;
static volatile uint32_t toggle_tick_counter = 0;

// 재생 시간 제어용 변수
static volatile uint32_t play_duration_ticks = 0;
static volatile uint32_t duration_tick_counter = 0;


// ===== 함수 정의 =====

void SPEAKER_Init(TIM_HandleTypeDef *htim) {
    // main.c에서 넘겨받은 타이머 핸들러를 내부 포인터 변수에 저장
    speaker_htim = htim;
    // 타이머 인터럽트 시작
    HAL_TIM_Base_Start_IT(speaker_htim);
}

void SPEAKER_Start(uint32_t frequency, uint32_t play_time_ms) {
    if (frequency == 0 || play_time_ms == 0) {
        return;
    }

    __disable_irq();

    uint32_t temp_toggle_ticks = SAMPLING_RATE / (frequency * 2);
    if (temp_toggle_ticks == 0)
    {
        temp_toggle_ticks = 1;
    }
    toggle_period_ticks = temp_toggle_ticks;

    play_duration_ticks = (uint64_t)SAMPLING_RATE * play_time_ms / 1000;

    toggle_tick_counter = 0;
    duration_tick_counter = 0;
    is_playing = 1;

    __enable_irq();
}

void SPEAKER_Stop(void) {
    is_playing = 0;
    HAL_GPIO_WritePin(Speaker_GPIO_Port, Speaker_Pin, GPIO_PIN_RESET);
}

void SPEAKER_Loop(void) {
    if (is_playing) {
        // 주파수 생성 로직
        toggle_tick_counter++;
        if (toggle_tick_counter >= toggle_period_ticks) {
            HAL_GPIO_TogglePin(Speaker_GPIO_Port, Speaker_Pin);
            toggle_tick_counter = 0;
        }

        // 재생 시간 카운트 로직
        duration_tick_counter++;
        if (duration_tick_counter >= play_duration_ticks) {
            SPEAKER_Stop();
        }
    }
}

uint8_t SPEAKER_IsPlaying(void) {
    return is_playing;
}
