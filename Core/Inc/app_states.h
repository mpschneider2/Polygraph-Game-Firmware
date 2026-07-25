/*
 * app_states.h
 *
 *  Created on: Jul 19, 2026
 *      Author: matthewschneider
 */

#ifndef INC_APP_STATES_H_
#define INC_APP_STATES_H_

typedef enum {
	APP_SPLASH,
	APP_RECORDING,
	APP_CALCULATING,
	APP_RESULTS
} app_state_t;

void app_states_init();
void app_states_run();

app_state_t app_states_get_state();


#endif /* INC_APP_STATES_H_ */
