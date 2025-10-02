
#include "seg7array.h"

// 각 자리 별 상태;  abcdefgp 순
static uint8_t cathodes[4] = {0b00000000, 0b00000000, 0b00000000, 0b00000000};

static GPIO_TypeDef *catPort[8] = {SEGa_GPIO_Port, SEGb_GPIO_Port, SEGc_GPIO_Port, SEGd_GPIO_Port,
                           SEGe_GPIO_Port, SEGf_GPIO_Port, SEGg_GPIO_Port, SEGp_GPIO_Port};

static const uint16_t catPin[8] = {SEGa_Pin, SEGb_Pin, SEGc_Pin, SEGd_Pin,
                          SEGe_Pin, SEGf_Pin, SEGg_Pin, SEGp_Pin};

static GPIO_TypeDef *posPort[4] = {SEG1_GPIO_Port, SEG2_GPIO_Port, SEG3_GPIO_Port, SEG4_GPIO_Port};
static const uint16_t posPin[4] = {SEG1_Pin, SEG2_Pin, SEG3_Pin, SEG4_Pin};


void SEG7ARRAY_Set_cathode(uint8_t pos, uint8_t cathode_bits) {
    if (pos > 5 || pos == 0) return;

    cathodes[pos-1] = cathode_bits;
}


void SEG7ARRAY_Cycle(void) {
	for (int pos = 0; pos < 4; pos++) {
		for (int cat = 0; cat < 8; cat++) {
			HAL_GPIO_WritePin(catPort[cat], catPin[cat], (GPIO_PinState)((cathodes[pos] >> (7 - cat)) & 1));
		}

		HAL_GPIO_WritePin(posPort[pos], posPin[pos], GPIO_PIN_RESET);

		HAL_Delay(1);

		HAL_GPIO_WritePin(posPort[pos], posPin[pos], GPIO_PIN_SET);
	}
}
