#include "main.h"
#include "buttons.h"
#include <stdbool.h>
#include <stdio.h>

static bool l_button_pressed = false;
static bool c_button_pressed = false;
static bool r_button_pressed = false;

static uint32_t last_l_button_pressed = 0;
static uint32_t last_c_button_pressed = 0;
static uint32_t last_r_button_pressed = 0;

#define DEB_TIME 50



void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin) {
	printf("some button pressed!\r\n");
	if (GPIO_Pin == L_BUTTON_Pin) {
		if (HAL_GetTick() - last_l_button_pressed > DEB_TIME) {
			last_l_button_pressed = HAL_GetTick();
			l_button_pressed = true;
		}
	}
	if (GPIO_Pin == C_BUTTON_Pin) {
		if (HAL_GetTick() - last_c_button_pressed > DEB_TIME) {
			last_c_button_pressed = HAL_GetTick();
			c_button_pressed = true;
		}
	}
	if (GPIO_Pin == R_BUTTON_Pin) {
		if (HAL_GetTick() - last_r_button_pressed > DEB_TIME) {
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

void clearButtons() {
	l_button_pressed = false;
	c_button_pressed = false;
	r_button_pressed = false;
}
