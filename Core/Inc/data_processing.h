/*
 * data_processing.h
 *
 *  Created on: Jul 20, 2026
 *      Author: matthewschneider
 */

#ifndef INC_DATA_PROCESSING_H_
#define INC_DATA_PROCESSING_H_

#define SAKOE_CHIBA_BAND 15

float data_processing_calc_mean(uint16_t * arr, uint16_t n);
float data_processing_calc_stdev(uint16_t * arr, uint16_t n, float * mean_pointer);
void data_processing_normalize(uint16_t * source_arr, float * dest_arr, uint16_t n);

uint16_t calculate_range(uint16_t * arr, uint16_t n);

void data_processing_calc_1hz_hrv(uint16_t * ibis, uint16_t n, float * RMSSD, uint16_t * out_n);;

int min(int a, int b);


int max(int a, int b);

float dist(float a, float b);

float data_processing_DTW(float * arr_a, float * arr_b, uint16_t a_n, uint16_t b_n);


#endif /* INC_DATA_PROCESSING_H_ */
