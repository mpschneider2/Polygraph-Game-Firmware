/*
 * filters.h
 *
 *  Created on: Jul 19, 2026
 *      Author: matthewschneider
 */

#ifndef INC_FILTERS_H_
#define INC_FILTERS_H_

#define MAX_SAMPLES 8

typedef struct {
	uint16_t raw_data[MAX_SAMPLES];
	uint16_t filtered_data[MAX_SAMPLES];
	uint8_t idx;
	uint32_t sum;
} FIR_filter_t;

void filter_init_FIR(FIR_filter_t * filter);
void filter_update_FIR(FIR_filter_t * filter, uint16_t val);

#endif /* INC_FILTERS_H_ */
