/*
 * gsr.h
 *
 *  Created on: Jul 24, 2026
 *      Author: matthewschneider
 */

#ifndef INC_GSR_H_
#define INC_GSR_H_

#include "game_constants.h"
#include "main.h"
#include <stdbool.h>

#define gsr_sample_rate 1 //in hertz. 1 is a good starting point, may need to increase later... but that involves modifying DTW algorithm
#define extra_samples 5// add a little buffer so there's no race condition before overwriting so that idx can still be used to calculate everything if time is shortened

typedef struct {
	uint16_t data[gsr_sample_rate*60*MAX_DURATION+extra_samples]; // set for 5 min sample time
	bool enabled;
	uint16_t counter; // used in downsampling
	uint16_t idx;
	uint32_t sum;

} gsr_instance_t;

void gsr_init(gsr_instance_t * gsr);
void gsr_run(gsr_instance_t * gsr, uint16_t val);

void gsr_start(gsr_instance_t * gsr);
void gsr_stop(gsr_instance_t * gsr) ;

#endif /* INC_GSR_H_ */
