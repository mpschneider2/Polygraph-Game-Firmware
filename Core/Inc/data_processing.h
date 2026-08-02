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

void data_processing_clean_ibi(uint16_t * ibis, uint16_t n);
//DEFINES FOR THAT FUNCTION

#define REFWINDOW 5 // just for now, sample last 5 to find a filler value for IBI that's viable. This sort of messes up dynamic time warping
	// but that's okay since it has a fairly wide window (for now). As seen in the MATLAB graphs, there is higher HRV correlation than GSR correlation, so can get away with wider
	// bounds JUST for HRV maybe. Also may not be necessary once a shroud is implemented.
#define LBOUND 300
#define HBOUND 2000

uint8_t data_processing_combined_score(float hrv_dtw_score, float gsr_dtw_score);

#endif /* INC_DATA_PROCESSING_H_ */
