/*
 * usart2console.c
 *
 *  Created on: Sep 5, 2025
 *      Author: UOS
 */

#include "usart2console.h"
#include <string.h>

/**
 * @brief UART 수신 데이터를 임시로 저장하는 링 버퍼(Ring Buffer)입니다.
 * @note
 *   - 생산자: UART 수신 인터럽트(`U2C_RxCpltCallback`)가 데이터를 여기에 씁니다.
 *   - 소비자: 메인 루프의 `U2C_process` 함수가 여기서 데이터를 읽어갑니다.
 */
static uint8_t rx_buffer[U2C_RX_BUFFER_SIZE];

/**
 * @brief 링 버퍼의 head 인덱스입니다.
 * @note 데이터가 써질 다음 위치를 가리킵니다.
 */
static volatile uint16_t rx_head = 0;

/**
 * @brief 링 버퍼의 tail 인덱스입니다.
 * @note 소비자가 읽어야 할 데이터의 위치를 가리킵니다.
 */
static uint16_t rx_tail = 0;

/**
 * @brief HAL 라이브러리의 UART 수신 함수가 사용할 1바이트 저장 공간입니다.
 * @note 인터럽트가 발생할 때마다 수신된 1바이트 데이터가 여기에 임시로 저장됩니다.
 */
static uint8_t rx_byte;



/**
 * @brief `U2C_process` 함수가 링 버퍼에서 데이터를 꺼내 완성된 한 줄의 명령어를 만들기 위한 버퍼입니다.
 */
static uint8_t cmd_buffer[U2C_RX_BUFFER_SIZE];

/**
 * @brief `cmd_buffer`에 현재까지 입력된 문자의 개수(커서 위치)를 나타냅니다.
 */
static uint16_t cmd_idx = 0;


/**
 * @brief UART가 현재 전송 중인지 여부를 나타내는 플래그입니다.
 * @note `U2C_print`가 호출될 때 `true`로 설정되고, 전송 완료 콜백(`HAL_UART_TxCpltCallback`)에서 `false`로 설정됩니다.
 */
static volatile bool tx_busy = false;



/**
 * @brief U2C(USART to Console) 라이브러리를 초기화합니다.
 * @note 이 함수는 main 함수의 `while(1)` 루프 이전에 한 번만 호출되어야 합니다.
 */
void U2C_init(void) {
    // HAL 라이브러리를 통해 UART 채널에서 1바이트 비동기(인터럽트 방식) 수신을 시작합니다.
    // 데이터가 1바이트 수신될 때마다 `HAL_UART_RxCpltCallback`이 호출됩니다.
    HAL_UART_Receive_IT(U2C_USART_CHANNEL, &rx_byte, 1);
}

/**
 * @brief [생산자] UART 수신 완료 콜백 함수입니다.
 * @note
 *   - 이 함수는 `stm32f4xx_it.c` 파일의 `HAL_UART_RxCpltCallback` 함수 안에서 반드시 호출되어야 합니다.
 *   - 역할: UART 하드웨어로부터 수신된 1바이트를 링 버퍼(`rx_buffer`)에 저장하는 '생산자'의 역할을 합니다.
 *   - 중요: 인터럽트 서비스 루틴의 일부이므로, 코드는 **최대한 빠르고 간결해야 합니다.**
 */
void U2C_RxCpltCallback(void) {
    // 1. 링 버퍼에 데이터 저장 (핵심 로직)

    // head가 다음으로 이동할 위치를 계산합니다.
    // 버퍼 크기로 나눈 나머지 연산(%)을 통해 인덱스가 버퍼 끝에 도달하면 자동으로 0으로 돌아가게(wrap-around) 만듭니다.
    uint16_t next_head = (rx_head + 1) % U2C_RX_BUFFER_SIZE;

    // 다음 head 위치가 현재 tail 위치와 같은지 확인합니다.
    // 만약 같다면, 이는 링 버퍼가 꽉 찼다는 의미입니다. (소비자가 데이터를 충분히 빨리 읽어가지 못함)
    if (next_head == rx_tail) {
        // 버퍼가 가득 찼을 때의 처리 정책을 여기에 정의할 수 있습니다.
        // 예: 에러 카운터를 증가시키거나, 특정 LED를 켜는 등.
        // 지금은 새로운 데이터를 버려서(무시해서) 버퍼의 내용을 보호합니다.
    }
    else {
        // 버퍼에 공간이 있으므로, 수신된 바이트(`rx_byte`)를 현재 head 위치에 저장합니다.
        rx_buffer[rx_head] = rx_byte;
        // head 인덱스를 다음 위치로 업데이트합니다.
        rx_head = next_head;
    }

    // 2. 다음 바이트 수신 준비

    // 다음 1바이트를 계속 수신하기 위해 HAL 수신 인터럽트를 다시 활성화합니다.
    HAL_UART_Receive_IT(U2C_USART_CHANNEL, &rx_byte, 1);
}


/**
 * @brief [소비자] 수신된 데이터를 처리하는 함수입니다.
 * @note
 *   - 이 함수는 `main` 함수의 `while(1)` 루프 안에서 계속해서 호출되어야 합니다.
 *   - 역할: 링 버퍼에 쌓인 데이터를 읽어가서, 한 줄의 명령어로 만들고 처리하는 '소비자'의 역할을 합니다.
 */
void U2C_process(void) {
    // 1. 링 버퍼에 처리할 데이터가 있는지 확인

    // head와 tail이 같지 않다는 것은, 생산자(인터럽트)가 버퍼에 써놓은 데이터가 있다는 의미입니다.
    while (rx_head != rx_tail) {

        // 2. 링 버퍼에서 데이터 1바이트 읽기

        // tail이 가리키는 위치에서 문자 하나를 꺼냅니다.
        uint8_t c = rx_buffer[rx_tail];
        // tail 인덱스를 다음 위치로 업데이트합니다. (wrap-around 처리 포함)
        rx_tail = (rx_tail + 1) % U2C_RX_BUFFER_SIZE;


        // 3. 읽어온 문자 종류에 따라 커맨드 라인 편집

        // 백스페이스(ASCII 0x08) 또는 DEL(ASCII 0x7F) 처리
        if (c == '\b' || c == 0x7F) {
            if (cmd_idx > 0) { // 커맨드 버퍼에 문자가 있을 때만 동작
                cmd_idx--; // 커맨드 버퍼 인덱스를 하나 줄입니다.
                // 터미널에서도 문자를 지우는 효과를 주기 위해 "백스페이스-스페이스-백스페이스" 시퀀스를 전송합니다.
                U2C_print((uint8_t*)"\b \b", 3);
            }
        }

        // 엔터(Enter) 키 처리 (CR: Carriage Return, LF: Line Feed)
        else if (c == '\r' || c == '\n') {
            // 현재 커맨드 버퍼에 내용이 있을 때만 커맨드를 처리합니다.
            if (cmd_idx > 0) {
                // 터미널에 줄바꿈 문자를 보내 커서를 다음 줄로 내립니다.
                U2C_print((uint8_t*) NEWLINE_CHARACTER, strlen(NEWLINE_CHARACTER));

                // --- 커맨드 처리 로직 ---
                // 여기서는 간단히 수신된 명령어를 "CMD: "와 함께 다시 출력합니다.
                // 실제 애플리케이션에서는 이 부분에서 `strcmp` 등으로 명령어를 비교하여
                // 특정 동작(예: LED 켜기/끄기)을 수행하는 코드가 들어갑니다.
                U2C_print((uint8_t*)"CMD: ", 5);
                U2C_println(cmd_buffer, cmd_idx);
                // --- 커맨드 처리 로직 끝 ---

                // 다음 명령어를 수신하기 위해 커맨드 버퍼 인덱스를 0으로 리셋합니다.
                cmd_idx = 0;
            }
        }
        // 그 외 출력 가능한 일반 문자 처리
        else if (c >= ' ') {
            // 커맨드 버퍼가 꽉 차지 않았는지 확인합니다.
            if (cmd_idx < U2C_RX_BUFFER_SIZE - 1) {
                // 수신된 문자를 터미널에 그대로 보여줍니다 (입력 에코).
                U2C_print(&c, 1);
                // 커맨드 버퍼에도 문자를 저장하고, 인덱스를 증가시킵니다.
                cmd_buffer[cmd_idx++] = c;
            }
        }
    } // while (rx_head != rx_tail)
}


/**
 * @brief U2C 콘솔에 문자열을 출력합니다. (비동기 방식)
 * @param pString 출력할 문자열 포인터
 * @param length 출력할 문자열 길이
 */
HAL_StatusTypeDef U2C_print(uint8_t *pString, uint16_t length) {
    // 전송이 이미 진행 중인지 확인합니다.
    // 만약 busy 상태이면, U2C_TX_TIMEOUT_MS 만큼 기다립니다.
    uint32_t tickstart = HAL_GetTick();
    while (tx_busy) {
        if ((HAL_GetTick() - tickstart) > U2C_TX_TIMEOUT_MS) {
            return HAL_BUSY; // 타임아웃 시간이 지나도 busy 상태이면 에러를 반환합니다.
        }
    }

    tx_busy = true; // 전송 시작을 알리기 위해 플래그를 true로 설정합니다.
    // HAL 라이브러리를 통해 비동기(인터럽트 방식) UART 전송을 시작합니다.
    return HAL_UART_Transmit_IT(U2C_USART_CHANNEL, pString, length);
}

/**
 * @brief U2C 콘솔에 문자열을 출력하고, 자동으로 줄바꿈 문자를 추가합니다.
 * @param pString 출력할 문자열 포인터
 * @param length 출력할 문자열 길이
 */
HAL_StatusTypeDef U2C_println(uint8_t *pString, uint16_t length) {
    // 'static' 키워드를 사용하여 버퍼를 선언합니다.
    // 이 버퍼는 함수가 리턴되어도 파괴되지 않고 메모리에 계속 남아있습니다.
    // 따라서 비동기 전송(IT/DMA)이 완료될 때까지 데이터가 안전하게 보존됩니다.
    static uint8_t println_buffer[U2C_TX_BUFFER_SIZE];

    if (length + 2 > U2C_TX_BUFFER_SIZE) {
        return HAL_ERROR; // 합쳐진 문자열이 버퍼보다 깁니다.
    }

    // static 버퍼에 최종 문자열(원본 + \r\n)을 만듭니다.
    memcpy(println_buffer, pString, length);
    println_buffer[length] = '\r';
    println_buffer[length+1] = '\n';

    // U2C_print 함수를 호출합니다.
    // 이제 전송이 시작된 후 이 함수가 리턴되어도, println_buffer는 메모리에 안전하게 남아있습니다.
    return U2C_print(println_buffer, length + 2);
}

/**
 * @brief UART 전송 완료 콜백 함수입니다.
 * @note
 *   - 이 함수는 `stm32f4xx_it.c` 파일의 `HAL_UART_TxCpltCallback` 함수 안에서 반드시 호출되어야 합니다.
 *   - `HAL_UART_Transmit_IT`로 시작된 데이터 전송이 완료되면 호출됩니다.
 */
void U2C_TxCpltCallback(void) {
    tx_busy = false; // 전송이 끝났으므로 플래그를 false로 설정하여 다른 데이터가 전송될 수 있도록 합니다.
}
