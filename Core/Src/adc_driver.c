/*
 * adc_driver.c
 *
 *  Created on: Jul 19, 2026
 *      Author: matthewschneider
 */

#include "main.h"
#include "adc_driver.h"
#include "ppg_processor.h"
#include "gsr.h"

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;

static ppg_instance_t * player_a_ppg = NULL;
static ppg_instance_t * player_b_ppg = NULL;
static gsr_instance_t * player_a_gsr = NULL;
static gsr_instance_t * player_b_gsr = NULL;

uint16_t adcVals[7];

void adc_driver_init(ppg_instance_t * a_ppg, ppg_instance_t * b_ppg, gsr_instance_t * a_gsr, gsr_instance_t * b_gsr) {
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&adcVals,7);

	player_a_ppg = a_ppg;
	player_b_ppg = b_ppg;
	player_a_gsr = a_gsr;
	player_b_gsr = b_gsr;

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	//there's only one adc!
	if (player_a_ppg != NULL && player_b_ppg != NULL && player_a_gsr != NULL && player_b_gsr != NULL) {
		ppg_processor_run(player_a_ppg, adcVals[5]);
		ppg_processor_run(player_b_ppg, adcVals[0]);

		gsr_run(player_a_gsr, adcVals[3]);
		gsr_run(player_b_gsr, adcVals[2]);
	}

}

void adc_driver_resume_adc() {
	HAL_TIM_Base_Start(&htim2);
}

void adc_driver_pause_adc() {
	HAL_TIM_Base_Stop(&htim2);
}
