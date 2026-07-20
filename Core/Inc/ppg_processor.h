/*
 * ppg_processor.h
 *
 *  Created on: Jul 19, 2026
 *      Author: matthewschneider
 */

#ifndef INC_PPG_PROCESSOR_H_
#define INC_PPG_PROCESSOR_H_

#include "filters.h"

#define MAX_DURATION 5 //in minutes
#define SAMPLE_PERIOD 2.0 // in millis

typedef enum {
	PPG_WAITING,
	PPG_SEARCHING,
	PPG_CALCULATING,
	PPG_DISABLED

} ppg_state_t;

typedef struct {
	FIR_filter_t filter;
	uint16_t data[MAX_DURATION * 200]; // max 200 bpm
	uint16_t idx;
	uint8_t search_cycles;
	uint16_t yleft, y_mid, y_right;
	uint32_t lowest_time;
	uint32_t last_beat;
	ppg_state_t state;
} ppg_instance_t;

void ppg_processor_init(ppg_instance_t ppg_instance);
void ppg_processor_run(ppg_instance_t ppg_instance);

#endif /* INC_PPG_PROCESSOR_H_ */
