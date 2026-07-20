/*
 * filters.c
 *
 *  Created on: Jul 19, 2026
 *      Author: matthewschneider
 */
#include "main.h"
#include "filters.h"
#include "string.h"

void filter_init_FIR(FIR_filter_t * filter) {
	memset(filter->raw_data, 0, sizeof(filter->raw_data));
	memset(filter->filtered_data, 0, sizeof(filter->filtered_data));
	filter->idx = 0;
	filter->sum = 0;
}

void filter_update_FIR(FIR_filter_t * filter, uint16_t val) {
	filter->sum -= filter->raw_data[filter->idx];
	filter->raw_data[filter->idx] = val;
	filter->sum += val;
	filter->filtered_data[filter->idx] = filter->sum / MAX_SAMPLES;
	filter->idx = (filter->idx + 1) % MAX_SAMPLES;
}
