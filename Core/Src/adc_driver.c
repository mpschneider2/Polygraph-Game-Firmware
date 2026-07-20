/*
 * adc_driver.c
 *
 *  Created on: Jul 19, 2026
 *      Author: matthewschneider
 */

#include "main.h"
#include "adc_driver.h"
#include "ppg_processor.h"

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;

static ppg_instance_t * player_a_ppg = NULL;
static ppg_instance_t * player_b_ppg = NULL;

uint16_t adcVals[7];

void adc_driver_init(ppg_instance_t * a, ppg_instance_t * b) {
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&adcVals,7);

	player_a_ppg = a;
	player_b_ppg = b;

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	//there's only one adc!
	if (player_a_ppg != NULL && player_b_ppg != NULL) {
		ppg_processor_run(player_a_ppg, adcVals[5]);
		ppg_processor_run(player_b_ppg, adcVals[0]);
	}

}

void adc_driver_resume_adc() {
	HAL_TIM_Base_Start_IT(&htim2);
}

void adc_driver_pause_adc() {
	HAL_TIM_Base_Stop(&htim2);
}
