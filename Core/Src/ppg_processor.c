/*
 * ppg_processor.c
 *
 *  Created on: Jul 19, 2026
 *      Author: matthewschneider
 */

#include "main.h"
#include "ppg_processor.h"
#include "filters.h"
#include <stdlib.h>

void ppg_processor_init(ppg_instance_t ppg_instance) {
	memset(ppg_instance->data, sizeof(ppg_instance->data)); // max 200 bpm
	ppg_instance->idx = 0;
	ppg_instance->search_cycles = 0;
	ppg_instance->last_beat = 0;
	ppg_instance->state = PPG_DISABLED;
	filter_init_FIR(ppg_instance->filter);
	yleft = y_mid = y_right = 0;
	lowest_time = 0;
	last_beat = 0;

}
void ppg_processor_run(ppg_instance_t ppg_instance, uint16_t val) {

	if (ppg_instance->state != PPG_DISABLED) {
		filter_update_FIR(ppg_instance->filter, val);
		switch (ppg_instance->state) {
			case PPG_WAITING:
				float voltage_delta = ppg_instance->filter->filtered_data[ppg_instance->filter->idx]/4095.0f*3.3 - ppg_instance->filter->filtered_data[(ppg_instance->filter->idx-4+MAX_SAMPLES)%MAX_SAMPLES]/4095.0f*3.3; //same as checking slope since we look for fixed period
				if (voltage_delta < -0.02) {
					ppg_instance->state = PPG_SEARCHING;
				}
				break;
			case PPG_SEARCHING:
				uint8_t curr_idx = (ppg_instance->filter->idx+MAX_SAMPLES-1)%MAX_SAMPLES;
				uint8_t prev_idx = (ppg_instance->filter->idx+MAX_SAMPLES-2)%MAX_SAMPLES;
				uint8_t next_idx = (ppg_instance->filter->idx);

				if (ppg_instance->search_cycles < 100) {
					if (ppg_instance->filter->filtered_data[curr_idx] < ppg_instance->y_mid) {
						ppg_instance->y_mid = ppg_instance->filter->filtered_data[curr_idx];
						ppg_instance->y_left = ppg_instance->filter->filtered_data[prev_idx];
						ppg_instance->y_right = ppg_instance->filter->filtered_data[next_idx];

						ppg_instance->lowest_time = HAL_GetTick();
					}
					search_cycles++;
				} else {
					ppg_instance->search_cycles = 0;
					ppg_instance->state = PPG_CALCULATING;
				}
				break;
			case PPG_CALCULATING:
				float

		}
	}

}

void ppg_processor_start(ppg_instance_t ppg_instance) {
	ppg_instance->state = PPG_WAITING;
}

void ppg_processor_stop(ppg_instance_t ppg_instance) {
	ppg_instance->state = PPG_DISABLED;
}


