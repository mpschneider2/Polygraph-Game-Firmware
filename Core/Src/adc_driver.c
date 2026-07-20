/*
 * adc_driver.c
 *
 *  Created on: Jul 19, 2026
 *      Author: matthewschneider
 */

#include "main.h"
#include "adc_driver.h"

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;

uint16_t adcVals[6];

void adc_driver_init() {
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&adcVals,7);

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	//there's only one adc!

}

void adc_driver_resume_adc() {
	HAL_TIM_Base_Start_IT(&htim2);
}

void adc_driver_pause_adc() {
	HAL_TIM_Base_Stop(&htim2);
}
