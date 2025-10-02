/**
  ******************************************************************************
  * @file           : keypad16.c
  * @brief          : 4x4 Keypad driver source file.
  * @author         : Gemini
  * @date           : 2025-09-23
  ******************************************************************************
  */

#include "keypad16.h"
#include <string.h> // memset, memcpy 함수 사용을 위해 포함

/* Private-like variables (static) */

// 4x4 키패드 문자 맵핑 (사용자 정의 가능)
static const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// 행(Row) 포트와 핀을 배열로 관리하여 스캔 로직을 간결하게 만듭니다.
static GPIO_TypeDef* row_ports[4] = {R1_PORT, R2_PORT, R3_PORT, R4_PORT};
static const uint16_t row_pins[4] = {R1_PIN, R2_PIN, R3_PIN, R4_PIN};

// 열(Column) 포트와 핀을 배열로 관리합니다.
static GPIO_TypeDef* col_ports[4] = {C1_PORT, C2_PORT, C3_PORT, C4_PORT};
static const uint16_t col_pins[4] = {C1_PIN, C2_PIN, C3_PIN, C4_PIN};

// 현재 키패드의 물리적인 눌림 상태를 저장하는 변수 (0: 안눌림, 1: 눌림)
static uint8_t key_state[4][4];
// 이전 스캔 시점의 키패드 상태를 저장하는 변수 (Rising Edge 감지를 위함)
static uint8_t last_key_state[4][4];

// 스캔을 통해 확인된 '계속 눌리고 있는' 키
static char pressed_key = NO_KEY_PRESSED;
// 스캔을 통해 확인된 '새롭게 눌린' 키
static char triggered_key = NO_KEY_PRESSED;


/**
 * @brief 키패드 드라이버를 초기화합니다.
 */
void KEYPAD16_Init(void)
{
    // 모든 키 상태를 '안눌림'으로 초기화합니다.
    memset(key_state, 0, sizeof(key_state));
    memset(last_key_state, 0, sizeof(last_key_state));
    pressed_key = NO_KEY_PRESSED;
    triggered_key = NO_KEY_PRESSED;
}

/**
 * @brief 키패드 매트릭스를 스캔하여 현재 눌린 키를 감지합니다.
 */
void KEYPAD16_Scan(void)
{
    // 스캔 시작 전, 현재 키 상태를 이전 상태로 복사합니다.
    // 이를 통해 현재 스캔과 이전 스캔을 비교하여 '새롭게 눌린 키'를 감지할 수 있습니다.
    memcpy(last_key_state, key_state, sizeof(key_state));

    // 이번 스캔 주기의 결과 변수들을 초기화합니다.
    pressed_key = NO_KEY_PRESSED;
    triggered_key = NO_KEY_PRESSED;

    // 각 행(Row)을 순차적으로 스캔합니다.
    for (int r = 0; r < 4; ++r)
    {
        // 1. 현재 스캔할 행(r)에만 LOW 신호를 출력합니다.
        //    (다른 모든 행은 CubeMX 설정에 따라 기본적으로 HIGH 상태여야 하지만,
        //     안정성을 위해 모든 행을 HIGH로 설정 후 현재 행만 LOW로 설정합니다.)
        HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(row_ports[r], row_pins[r], GPIO_PIN_RESET); // 현재 행만 LOW

        // 2. 모든 열(Column)의 입력 상태를 읽습니다.
        for (int c = 0; c < 4; ++c)
        {
            // 열(Column) 핀은 내부 풀업(Pull-up) 저항으로 인해 평소에는 HIGH 상태입니다.
            // 만약 키가 눌리면, LOW 신호를 출력 중인 행과 물리적으로 연결되어 LOW 상태가 됩니다.
            if (HAL_GPIO_ReadPin(col_ports[c], col_pins[c]) == GPIO_PIN_RESET)
            {
                // --- 키 눌림 감지됨 ---

                // 현재 키의 물리적 상태를 '눌림(1)'으로 기록합니다.
                key_state[r][c] = 1;

                // '계속 눌리고 있는 키' 변수에 현재 키 문자를 저장합니다.
                // (만약 다른 키가 이어서 눌리면 이 값은 덮어쓰여집니다.)
                pressed_key = keymap[r][c];

                // '새롭게 눌린 키(Rising Edge)'인지 확인합니다.
                // 이전 스캔(last_key_state)에서는 '안눌림(0)'이었고,
                // 현재 스캔(key_state)에서 '눌림(1)'이면 새로운 입력입니다.
                if (last_key_state[r][c] == 0)
                {
                    triggered_key = keymap[r][c];
                }
            }
            else
            {
                // --- 키 안눌림 감지됨 ---
                // 현재 키의 물리적 상태를 '안눌림(0)'으로 기록합니다.
                key_state[r][c] = 0;
            }
        }
    }
    
    // 3. 스캔이 끝난 후 모든 행을 다시 HIGH로 설정하여 다음 스캔을 준비합니다.
    HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_SET);
}

/**
 * @brief 현재 '계속 누르고 있는' 키의 문자를 반환합니다.
 */
char KEYPAD16_Get_Pressed_Key(void)
{
    return pressed_key;
}

/**
 * @brief 키가 '처음 눌리는 순간'에만 한 번 키의 문자를 반환합니다.
 */
char KEYPAD16_Get_Triggered_Key(void)
{
    return triggered_key;
}