/*
 * ppg_processor.h
 *
 *  Created on: Jul 19, 2026
 *      Author: matthewschneider
 */

#ifndef INC_PPG_PROCESSOR_H_
#define INC_PPG_PROCESSOR_H_

#include "filters.h"
#include <stdbool.h>
#include "game_constants.h"

//#define MAX_DURATION 5 //in minutes
// now defined in game_constants.h
#define SAMPLE_PERIOD 2.5 // in millis
#define BUFFER_LENGTH (MAX_DURATION * 200) // max 200 bpm

typedef enum {
	PPG_WAITING,
	PPG_SEARCHING,
	PPG_CALCULATING,
	PPG_DISABLED

} ppg_state_t;


typedef struct {
	FIR_filter_t filter;
	volatile uint16_t data[BUFFER_LENGTH]; // max 200 bpm
	volatile uint16_t idx;
	uint8_t search_cycles;
	uint16_t y_left, y_mid, y_right;
	uint32_t lowest_time;
	volatile uint32_t last_beat;
	volatile ppg_state_t state;
	volatile bool new_data;
} ppg_instance_t;

void ppg_processor_init(ppg_instance_t * ppg_instance);
void ppg_processor_run(ppg_instance_t * ppg_instance, uint16_t val);

void ppg_processor_start(ppg_instance_t * ppg_instance);
void ppg_processor_stop(ppg_instance_t * ppg_instance);

#endif /* INC_PPG_PROCESSOR_H_ */
