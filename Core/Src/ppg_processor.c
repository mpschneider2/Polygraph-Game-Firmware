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
#include <string.h>
#include <stdio.h>

void ppg_processor_init(ppg_instance_t * ppg_instance) {
	memset(ppg_instance->data, 0, sizeof(ppg_instance->data)); // max 200 bpm
	ppg_instance->idx = 0;
	ppg_instance->search_cycles = 0;
	ppg_instance->last_beat = 0;
	ppg_instance->state = PPG_DISABLED;
	filter_init_FIR(&(ppg_instance->filter));
	ppg_instance->y_left = ppg_instance->y_mid = ppg_instance->y_right = UINT16_MAX;
	ppg_instance->lowest_time = 0;
	ppg_instance->last_beat = 0;

}
void ppg_processor_run(ppg_instance_t * ppg_instance, uint16_t val) {

	if (ppg_instance->state != PPG_DISABLED) {
		filter_update_FIR(&ppg_instance->filter, val);
		switch (ppg_instance->state) {
			case PPG_WAITING:
				if (HAL_GetTick() - ppg_instance->last_beat < 300) return;
				float voltage_delta = ppg_instance->filter.filtered_data[ppg_instance->filter.idx-1]/4095.0f*3.3 - ppg_instance->filter.filtered_data[(ppg_instance->filter.idx-4-1+MAX_SAMPLES)%MAX_SAMPLES]/4095.0f*3.3; //same as checking slope since we look for fixed period. need to check for 20 mV voltage delta in 10ms gap
				if (voltage_delta < -0.02) { // original -0.02
					ppg_instance->state = PPG_SEARCHING;
				}
				break;
			case PPG_SEARCHING:
				printf("Start searching!\r\n");
				uint8_t curr_idx = (ppg_instance->filter.idx+MAX_SAMPLES-1-1)%MAX_SAMPLES;
				uint8_t prev_idx = (ppg_instance->filter.idx+MAX_SAMPLES-2-1)%MAX_SAMPLES;
				uint8_t next_idx = (ppg_instance->filter.idx-1);

				if (ppg_instance->search_cycles < 80) { // 200ms window / 2.5ms period
					if (ppg_instance->filter.filtered_data[curr_idx] < ppg_instance->y_mid) {
						ppg_instance->y_mid = ppg_instance->filter.filtered_data[curr_idx];
						ppg_instance->y_left = ppg_instance->filter.filtered_data[prev_idx];
						ppg_instance->y_right = ppg_instance->filter.filtered_data[next_idx];

						ppg_instance->lowest_time = HAL_GetTick();
					}
					ppg_instance->search_cycles++;
				} else {
					ppg_instance->search_cycles = 0;
					ppg_instance->state = PPG_CALCULATING;
				}
				break;
			case PPG_CALCULATING:
				uint32_t new_time;
				if (2*ppg_instance->y_mid - ppg_instance->y_left - ppg_instance->y_right > 0) {
					new_time = ppg_instance->lowest_time + ((float) ppg_instance->y_right - (float) ppg_instance->y_left)/2.0f/(2.0f*ppg_instance->y_mid - (float) ppg_instance->y_left - (float) ppg_instance->y_right)*SAMPLE_PERIOD;
				} else {
					new_time = ppg_instance->lowest_time;
				}
				ppg_instance->data[ppg_instance->idx] = new_time - ppg_instance->last_beat;
				ppg_instance->last_beat = new_time;
				ppg_instance->new_data = true;
				ppg_instance->idx = (ppg_instance->idx + 1)%(BUFFER_LENGTH); //rewrites instead of overflow
				ppg_instance->state = PPG_WAITING;
				ppg_instance->y_mid = UINT16_MAX; // reset to high value
				printf("Beat.\r\n");

				break;
			case PPG_DISABLED:
				return;

		}
	}

}

void ppg_processor_start(ppg_instance_t * ppg_instance) {
	ppg_instance->state = PPG_WAITING;
}

void ppg_processor_stop(ppg_instance_t * ppg_instance) {
	ppg_instance->state = PPG_DISABLED;
}


