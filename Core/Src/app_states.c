/*
 * app_states.c
 *
 *  Created on: Jul 19, 2026
 *      Author: matthewschneider
 */
#include "main.h"
#include "app_states.h"
#include "gc9a01a.h"
#include "adc_driver.h"
#include "ppg_processor.h"
#include "U8g2_for_Adafruit_GFX.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "buttons.h"
#include "gsr.h"
#include "game_constants.h"

app_state_t current_app_state = APP_RECORDING;

static ppg_instance_t player_a_ppg;
static ppg_instance_t player_b_ppg;

static gsr_instance_t player_a_gsr;
static gsr_instance_t player_b_gsr;

static uint32_t countdown_start = 0;
static uint16_t countdown_secs = MAX_DURATION*60; // mins to secs

extern SPI_HandleTypeDef hspi1;

GC9A01A tft1;
u8g2_font_t font;

static bool state_updated = true; // used to detect whether it's the first run of the time in the state (critical for screen rendering)

void app_states_init() {
	current_app_state = APP_SPLASH;

	printf("Initializing display \r\n");

	GC9A01A_init(&tft1, &hspi1,
	  	  LCD_CS_GPIO_Port, LCD_CS_Pin,
	  	  LCD_DC_GPIO_Port, LCD_DC_Pin,
	  	  LCD_BLK_GPIO_Port, LCD_BLK_Pin,
	  	  LCD_RESET_GPIO_Port, LCD_RESET_Pin
	    );


	adc_driver_init(&player_a_ppg, &player_b_ppg, &player_a_gsr, &player_b_gsr);
	ppg_processor_init(&player_a_ppg);
	ppg_processor_init(&player_b_ppg);
	gsr_init(&player_a_gsr);
	gsr_init(&player_b_gsr);

}

void app_states_run() {
	bool l_pressed = check_l_button_pressed();
	bool c_pressed = check_c_button_pressed();
	bool r_pressed = check_r_button_pressed();

	if (state_updated) {
		printf("Changing state to: %d\r\n", current_app_state);
	}

	switch (current_app_state) {
		case APP_SPLASH:
			if (state_updated) {
				GC9A01A_fill(&tft1, GC9A01A_OLIVE);
				GC9A01A_Draw_Str("START", &tft1, 55, 141, u8g2_font_logisoso42_tr, GC9A01A_BLACK, GC9A01A_OLIVE, 100); // u8g2_font_tenthinnerguys_tu will be good for tiny text.
				state_updated = false;
			}

			if (l_pressed || c_pressed || r_pressed) {
				printf("button pressed\r\n");
				clearButtons();
				state_updated = true;
				current_app_state = APP_RECORDING;
			}
			break;
		case APP_RECORDING:

			if (state_updated) {
				GC9A01A_fill(&tft1, GC9A01A_OLIVE);
				adc_driver_resume_adc(); // start it if not started
				ppg_processor_start(&player_a_ppg);
				ppg_processor_start(&player_b_ppg);
				gsr_start(&player_a_gsr);
				gsr_start(&player_b_gsr);
				state_updated = false;
				GC9A01A_Draw_Str("BPM", &tft1, 190, 115, u8g2_font_logisoso16_tr, GC9A01A_BLACK, GC9A01A_OLIVE, 50);
				GC9A01A_Draw_Str("BPM", &tft1, 15, 115, u8g2_font_logisoso16_tr, GC9A01A_BLACK, GC9A01A_OLIVE, 50);
				//start countdown timer
				countdown_start = HAL_GetTick();
			}

			//handle timer display
			if ((HAL_GetTick() - countdown_start)/1000 != MAX_DURATION*60 - countdown_secs) {//rely on int truncation
				//then time is updated
				countdown_secs = MAX_DURATION*60 - (HAL_GetTick() - countdown_start)/1000;

				//and update display
				char formatted_timer[5] = "-:--";
				sprintf(formatted_timer, "%d:%02d", countdown_secs/60, countdown_secs%60);
				GC9A01A_Draw_Str(formatted_timer, &tft1, 70, 70, u8g2_font_logisoso42_tr, GC9A01A_BLACK, GC9A01A_OLIVE, 100);

				//check if time to move on to next stage
				if (countdown_secs == 0) {
					current_app_state = APP_CALCULATING;
					state_updated = true;

					ppg_processor_stop(&player_a_ppg);
					ppg_processor_stop(&player_b_ppg);

					gsr_stop(&player_a_gsr);
					gsr_stop(&player_b_gsr);

					break;
				}

				printf("Timer updated\r\n");

			}

			//HANDLE DATA ACQUISITION
			//ALL READS ARE DONE BY INTERRUPTS!
			if (player_a_ppg.new_data) {
				//use sample size of 5
//				__disable_irq(); // fucks up adc reads

				if (player_a_ppg.idx > 9) {
					uint32_t sum = 0;
					for (int i = 0; i<10; i++) {
						sum += player_a_ppg.data[player_a_ppg.idx-i-1]; //idx points to NEXT slot
					}
					uint16_t bpm = 60000 * 10 / sum;

//					__enable_irq();

					char bpm_a_formatted[4];
					sprintf(bpm_a_formatted, "%u", bpm);
//					GC9A01A_Draw_Str(bpm_formatted, &tft1, 65, 170, u8g2_font_logisoso92_tn, GC9A01A_BLACK, GC9A01A_OLIVE, 180);
					GC9A01A_Draw_Str(bpm_a_formatted, &tft1, 15, 167, u8g2_font_logisoso42_tr, GC9A01A_BLACK, GC9A01A_OLIVE, 100);
				}
				player_a_ppg.new_data = false;
			}

			if (player_b_ppg.new_data) {
							//use sample size of 5
			//				__disable_irq(); // fucks up adc reads

				if (player_b_ppg.idx > 9) {
					uint32_t sum = 0;
					for (int i = 0; i<10; i++) {
						sum += player_b_ppg.data[player_b_ppg.idx-i-1]; //idx points to NEXT slot
					}
					uint16_t bpm = 60000 * 10 / sum;

//					__enable_irq();

					char bpm_b_formatted[4];
					sprintf(bpm_b_formatted, "%u", bpm);
//					GC9A01A_Draw_Str(bpm_formatted, &tft1, 65, 170, u8g2_font_logisoso92_tn, GC9A01A_BLACK, GC9A01A_OLIVE, 180);
					GC9A01A_Draw_Str(bpm_b_formatted, &tft1, 180, 167, u8g2_font_logisoso42_tr, GC9A01A_BLACK, GC9A01A_OLIVE, 100);
				}
				player_b_ppg.new_data = false;
			}

			//HANDLE BUTTON PRESSES

			if (l_pressed || c_pressed || r_pressed) {
				printf("button pressed\r\n");
				clearButtons();
				state_updated = true;
				current_app_state = APP_CALCULATING;

				ppg_processor_stop(&player_a_ppg);
				ppg_processor_stop(&player_b_ppg);

				gsr_stop(&player_a_gsr);
				gsr_stop(&player_b_gsr);
			}

			break;
	}
}

app_state_t app_states_get_state() {
	return current_app_state;
}


