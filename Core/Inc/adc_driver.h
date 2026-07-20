/*
 * adc_driver.h
 *
 *  Created on: Jul 19, 2026
 *      Author: matthewschneider
 */

#ifndef INC_ADC_DRIVER_H_
#define INC_ADC_DRIVER_H_

void adc_driver_init();
void adc_driver_resume_adc();
void adc_driver_pause_adc();

//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);


#endif /* INC_ADC_DRIVER_H_ */
