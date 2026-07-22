/*
 * data_processing.c
 *
 *  Created on: Jul 20, 2026
 *      Author: matthewschneider
 */

#include "main.h"
#include "data_processing.h"
#include "ppg_processor.h"
#include <string.h>
#include <math.h>

float data_processing_calc_mean(uint16_t * arr, uint16_t n) {
	if (n == 0) return 0.0f; // handle empty array case

	float new_avg = (float) arr[0];
	for (int i = 1; i < n; i++) {
		new_avg = new_avg + ((float) arr[i]-new_avg)/((float)(i+1));
	}
	return new_avg;
}

float data_processing_calc_stdev(uint16_t * arr, uint16_t n, float * mean_pointer) { //sample, not population
	float mean = data_processing_calc_mean(arr, n);
	if (mean_pointer != NULL) *mean_pointer = mean; // avoid calculating mean multiple times for normalization algorithm

	if (n<=1) return 0;// divide by 0 error

	float variance = 0;
	for (int i = 0; i < n; i++) {
		float diff = ((float) arr[i] - mean);
		variance += diff * diff;
	}
	float stdevs = sqrtf(variance/(n-1));
	return stdevs;
}

void data_processing_normalize(uint16_t * source_arr, float * dest_arr, uint16_t n) {
	float mean;
	float stdevs = data_processing_calc_stdev(source_arr, n, &mean);
	if (stdevs > 0.0001f) { // prevent divide by 0
		for (int i = 0; i < n; i++) {
			dest_arr[i] = ((float)source_arr[i] - mean) / stdevs;
		}
	} else {
		// center if zero variance
		for (int i = 0; i < n; i++) {
			dest_arr[i] = 0.0f;
		}
	}
}

uint16_t data_processing_calculate_range(const uint16_t * arr, uint16_t n) {
	uint16_t max = arr[0];
	uint16_t min = arr[0];

	if (arr == NULL || n == 0) return 0;

	for (int i = 1; i < n; i++) {
		if (arr[i] > max) max = arr[i];
		if (arr[i] < min) min = arr[i];
	}

	return max-min;
}

void data_processing_calc_1hz_hrv(uint16_t * ibis, uint16_t n, float * RMSSD, uint16_t * out_n) {
	uint32_t abs_times[BUFFER_LENGTH]; // consider making static


	if (n <= 1 || ibis == NULL || RMSSD == NULL) return; //define all possible states

	abs_times[0] = ibis[0];

	//reconstruct in absolute times for easier second-by-second interpolation
	for (int i = 1; i < n; i++) {
		abs_times[i] = abs_times[i-1]+ibis[i];
	}

	uint32_t total_sec = abs_times[n - 1] / 1000; //convert millis to seconds, and make sure to end before the last 10 seconds so rest of code still works
	uint32_t end_time = (total_sec > 10) ? (total_sec - 10) : 0;

	uint16_t start_idx = 0;
	uint16_t end_idx = 0;

	int resample_ct;

	for (resample_ct = 0; resample_ct < end_time; resample_ct ++) {

		//search for proper start and stop indices
		//start index
		while (start_idx < n && (float) abs_times[start_idx]/1000.0f < (float) resample_ct) {
			start_idx++; //keep increasing
		}
		//end index
		while (end_idx < n && (float) abs_times[end_idx]/1000.0f < (float) resample_ct+10.0f) { //look in a 10 second window I suppose
			end_idx++; //keep increasing
		}
		//found outer bounds of window

		uint16_t num_samples = end_idx - start_idx; //end_idx not included
		if (n <= 1 || ibis == NULL || RMSSD == NULL) return;

		float sq_diff_total = 0;

		for (int i = start_idx + 1; i < end_idx; i++) {
			float diff = (float)ibis[i] - (float)ibis[i - 1];
			sq_diff_total += diff*diff;
		}

		if (num_samples <= 1.0f) {
			RMSSD[resample_ct] = (resample_ct > 0) ? RMSSD[resample_ct - 1] : 0.0f;
		} else {
			RMSSD[resample_ct] = sqrtf(sq_diff_total / ((float) num_samples-1.0f));
		}
	}

	*out_n = resample_ct;

}

int min(int a, int b) {
	return (a < b) ? a : b;
}


int max(int a, int b) {
	return (a > b) ? a : b;
}

float dist(float a, float b) {
	return fabsf(a-b);
}

float data_processing_DTW(float * arr_a, float * arr_b, uint16_t a_n, uint16_t b_n) {
	//ASSUMPTIONS:
	// - a and b are close-ish to the same length
	// - data is manually normalized first
	// - there will be at most a 15 sample delay in either direction (and downsampling data to 1 hz means 15 seconds).
	// - we only care about the final "cost" so only need to keep two rows in memory (of a shorter width at that!)

	//planning,
	//use floats, let a be x axis, b, be y axis
	//allow 15 second delay either side, meaning 30 second delay total or 31 samples (15 per side, 1 in middle).
	//arrays can be sized with this rather than actual size of data

	//format of array: infinity, data, data, data ... data, data, data, infinity

	if (a_n < 50 || b_n < 50) return INFINITY; //not enough data to do effectively

	uint16_t col_offset = 0; //start at 1 since r2 starts offset from r1 by 1

	#define MATRIX_SZ (2*SAKOE_CHIBA_BAND+1+2) // on either side centered at middle, plus infinities

	static float cost_r1[MATRIX_SZ]; // will never be larger than data due to above.
	static float cost_r2[MATRIX_SZ];

	for (int i = 1; i < MATRIX_SZ; i++) {
		cost_r1[i] = INFINITY;
		cost_r2[i] = INFINITY;
	}

	cost_r1[0] = 0;

	int y;
	int x;

	for (y = 1; y <= b_n; y++) { //rows // y tracks actual y value which is not in array

		cost_r2[0] = INFINITY;

		//because we are inheriting from row below which is initialized to infinity and we expand right one by one, don't need to set right bound
		cost_r2[min(MATRIX_SZ-1 /*final index 1 less than len*/,col_offset+SAKOE_CHIBA_BAND)] = INFINITY; // initialize outer bounds to infinity; should
		uint16_t offset = max(col_offset - SAKOE_CHIBA_BAND, 0); //effective offset of start
		for (x = 1; x < min(min(col_offset+SAKOE_CHIBA_BAND, MATRIX_SZ - 1), a_n - offset+1); x++) { //x tracks array x value, not actual value// increment up until last infinity value.

			uint16_t a_idx = x - 1 + offset;
			uint16_t b_idx = y - 1; // x and y are 0-indexed

			//normally is fmin(D_i-j,j-1 , D_i-1,j , D_i,j-1 ) // in row,col format
			//for first few rows when offset = 0, row  indices are as normal. it's only after that that it switches
			if (offset == 0) {
				//do normally!
				cost_r2[x] = dist(arr_a[a_idx],arr_b[b_idx]) + fmin(cost_r1[x-1], fmin(cost_r1[x],cost_r2[x-1]));

			} else {
				//assume offset by 1
				cost_r2[x] = dist(arr_a[a_idx],arr_b[b_idx]) + fmin(cost_r1[x], fmin(cost_r1[x+1],cost_r2[x-1])); // modify to assume row below is left shifted by 1
			}
		}

		//clear the trailing data
		for (int i = x; i < MATRIX_SZ; i++) {
			cost_r2[i] = INFINITY;
		}

		memcpy(cost_r1, cost_r2, sizeof(float)*MATRIX_SZ);

		col_offset++;
	}

	return cost_r2[x-1]; // return final calculated cost


}
