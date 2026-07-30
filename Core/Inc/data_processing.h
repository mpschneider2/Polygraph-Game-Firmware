/*
 * data_processing.h
 *
 *  Created on: Jul 20, 2026
 *      Author: matthewschneider
 */

#ifndef INC_DATA_PROCESSING_H_
#define INC_DATA_PROCESSING_H_

#define SAKOE_CHIBA_BAND 15

double data_processing_calc_mean(const float * arr, uint16_t n);
double data_processing_calc_stdev(const float * arr, uint16_t n, double * mean_pointer);
void data_processing_normalize(float * source_arr, float * dest_arr, uint16_t n);

double data_processing_calc_mean_d(const uint16_t * arr, uint16_t n);
double data_processing_calc_stdev_d(const uint16_t * arr, uint16_t n, double * mean_pointer);
void data_processing_normalize_d(uint16_t * source_arr, float * dest_arr, uint16_t n);

float data_processing_calc_range(const float * arr, uint16_t n);

void data_processing_calc_1hz_hrv(const uint16_t * ibis, uint16_t n, float * RMSSD, uint16_t * out_n);;

int min(int a, int b);


int max(int a, int b);

float dist(float a, float b);

float data_processing_DTW(const float * arr_a, const float * arr_b, uint16_t a_n, uint16_t b_n);


#endif /* INC_DATA_PROCESSING_H_ */
