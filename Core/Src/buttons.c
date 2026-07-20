#include "main.h"
#include "buttons.h"
#include <stdbool.h>

static bool l_button_pressed = false;
static bool c_button_pressed = false;
static bool r_button_pressed = false;

uint32_t last_l_button_pressed = 0;
uint32_t last_c_button_pressed = 0;
uint32_t last_r_button_pressed = 0;



void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin) {
	if (GPIO_Pin == L_BUTTON_Pin) {
		if (HAL_GetTick() - last_l_button_pressed > 50) {
			last_l_button_pressed = HAL_GetTick();
			l_button_pressed = true;
		}
	}
	if (GPIO_Pin == C_BUTTON_Pin) {
		if (HAL_GetTick() - last_c_button_pressed > 50) {
			last_c_button_pressed = HAL_GetTick();
			c_button_pressed = true;
		}
	}
	if (GPIO_Pin == R_BUTTON_Pin) {
		if (HAL_GetTick() - last_r_button_pressed > 50) {
			last_r_button_pressed = HAL_GetTick();
			r_button_pressed = true;
		}
	}
}

bool check_l_button_pressed() {
	return l_button_pressed;
}
bool check_c_button_pressed() {
	return c_button_pressed;
}
bool check_r_button_pressed() {
	return r_button_pressed;
}
