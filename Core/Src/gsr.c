/*
 * gsr.c
 *
 *  Created on: Jul 24, 2026
 *      Author: matthewschneider
 */

#include "gsr.h"
#include "main.h"
#include <string.h>
#include <stdbool.h>


void gsr_init(gsr_instance_t * gsr) {
	//clears data and initializes
	memset(gsr, 0, sizeof(*gsr));
}


void gsr_run(gsr_instance_t * gsr, uint16_t val) {
	//averages and dowsamples to reduce noise
	if (!gsr->enabled) return;

	gsr->sum += val;
	gsr->counter ++;

	if (gsr->counter >= MAX_SAMPLE_RATE) { //sample rate of adc.
		gsr->counter = 0;
		gsr->data[gsr->idx] = (uint16_t) ((float) gsr->sum / (float) MAX_SAMPLE_RATE);
		gsr->sum = 0;
		gsr->idx = (gsr->idx + 1) % (sizeof(gsr->data)/sizeof(gsr->data[0]));
	}
}

void gsr_start(gsr_instance_t * gsr) {
	gsr->enabled = true;
}

void gsr_stop(gsr_instance_t * gsr) {
	gsr->enabled = false;
}
