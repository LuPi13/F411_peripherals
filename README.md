# F411_peripherals
임베디드시스템응용및설계 수업에 사용되는 F411 보드에 쓰이는 코드 모음

## 1. USART2CONSOLE
### 1. 용도
현재 보드의 USB-B 포트가 USART2와 연결되어 있는 것을 활용하여, 각종 데이터들을 USB Serial Port를 통해 Serial Terminal에서 주고 받게 해줍니다.
### 2. 사용법
1. **USART2 Mode**: Asynchronous
2. **USART2 NVIC**: USART2 global interrupt 체크
3. **Code Generation** 후, `main.c`에 다음과 같이 작성

```c
/* USER CODE BEGIN Includes */
#include "usart2console.h"
/* USER CODE END Includes */

...

/* USER CODE BEGIN 2 */
U2C_init();
/* USER CODE END 2 */

/* Infinite loop */
/* USER CODE BEGIN WHILE */
while (1)
{
	U2C_process();
/* USER CODE END WHILE */

...

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == U2C_USART_CHANNEL) {
		U2C_RxCpltCallback();
	}
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == U2C_USART_CHANNEL) {
		U2C_TxCpltCallback();
	}
}
/* USER CODE END 4 */
```

4. **`usart2console.c`에 커맨드 처리 로직 작성**

```c
// --- 커맨드 처리 로직 ---
// 이곳을 수정하여 커스텀 커맨드 처리 로직을 구현하세요.
// 여기서는 간단히 수신된 명령어를 "CMD: "와 함께 다시 출력합니다.
// 실제 애플리케이션에서는 이 부분에서 `strcmp` 등으로 명령어를 비교하여
// 특정 동작(예: LED 켜기/끄기)을 수행하는 코드가 들어갑니다.
U2C_print((uint8_t*)"CMD: ", 5);
U2C_println(cmd_buffer, cmd_idx);
// --- 커맨드 처리 로직 끝 ---
```

5. **출력 함수**
	- `U2C_print(*pString, length)`: 입력받은 문자열 출력
	- `U2C_println(*pString, length)`: 입력받은 문자열 끝에 개행문자 붙여서 출력

6. **(필요시) `usart2console.h`에서 줄바꿈 등의 옵션 변경**