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
#include "data_processing.h"
#include "sd_util.h"

app_state_t current_app_state = APP_RECORDING;

static ppg_instance_t player_a_ppg;
static ppg_instance_t player_b_ppg;

static gsr_instance_t player_a_gsr;
static gsr_instance_t player_b_gsr;

static uint32_t countdown_start = 0;
static uint16_t countdown_secs = MAX_DURATION*60; // mins to secs


//could be local but up here for easier debugging/monitoring
static float hrv_a[60*MAX_DURATION+EXTRA_SAMPLES];
static float hrv_b[60*MAX_DURATION+EXTRA_SAMPLES];
static uint16_t hrv_a_n = 0;
static uint16_t hrv_b_n = 0;

static float gsr_a_normalized[60*MAX_DURATION+EXTRA_SAMPLES];
static float gsr_b_normalized[60*MAX_DURATION+EXTRA_SAMPLES];

static float hrv_dtw;
static float gsr_dtw;

extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim16;

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

	HAL_TIM_PWM_Start(&htim16, GPIO_PIN_8);

	adc_driver_init(&player_a_ppg, &player_b_ppg, &player_a_gsr, &player_b_gsr);
	sd_util_init();
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

				ppg_processor_init(&player_a_ppg);
				ppg_processor_init(&player_b_ppg);
				gsr_init(&player_a_gsr);
				gsr_init(&player_b_gsr);

				GC9A01A_fill(&tft1, BG_PRIMARY);
				GC9A01A_Draw_Str("START", &tft1, 55, 141, u8g2_font_logisoso42_tr, TEXT_PRIMARY, BG_PRIMARY, 100); // u8g2_font_tenthinnerguys_tu will be good for tiny text.
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
				GC9A01A_fill(&tft1, BG_PRIMARY);
				adc_driver_resume_adc(); // start it if not started
				ppg_processor_start(&player_a_ppg);
				ppg_processor_start(&player_b_ppg);
				gsr_start(&player_a_gsr);
				gsr_start(&player_b_gsr);
				state_updated = false;
				GC9A01A_Draw_Str("BPM", &tft1, 190, 115, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 50);
				GC9A01A_Draw_Str("BPM", &tft1, 15, 115, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 50);
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
				GC9A01A_Draw_Str(formatted_timer, &tft1, 70, 70, u8g2_font_logisoso42_tr, TEXT_PRIMARY, BG_PRIMARY, 100);

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
//					GC9A01A_Draw_Str(bpm_formatted, &tft1, 65, 170, u8g2_font_logisoso92_tn, TEXT_PRIMARY, BG_PRIMARY, 180);
					GC9A01A_Draw_Str(bpm_a_formatted, &tft1, 15, 167, u8g2_font_logisoso42_tr, TEXT_PRIMARY, BG_PRIMARY, 100);
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
//					GC9A01A_Draw_Str(bpm_formatted, &tft1, 65, 170, u8g2_font_logisoso92_tn, TEXT_PRIMARY, BG_PRIMARY, 180);
					GC9A01A_Draw_Str(bpm_b_formatted, &tft1, 180, 167, u8g2_font_logisoso42_tr, TEXT_PRIMARY, BG_PRIMARY, 100);
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
		case APP_CALCULATING:
			//check amount of data or return error
			if (state_updated) {
				if (player_a_ppg.idx < 40 || player_b_ppg.idx < 40 || player_a_gsr.idx < 60 || player_b_gsr.idx < 60) {
					//not enough data!
					GC9A01A_fill(&tft1, BG_PRIMARY);
					GC9A01A_Draw_Str("Not enough data.", &tft1, 40, 60, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 0);
					GC9A01A_Draw_Str("Please play again.", &tft1, 40, 80, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 0);
					GC9A01A_Draw_Str("Ensure heart rate sensor", &tft1, 8, 100, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 0);
					GC9A01A_Draw_Str("is reading fingers.", &tft1, 8, 120, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 0);
					GC9A01A_Draw_Str("Ensure both fingers are", &tft1, 8, 140, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 0);
					GC9A01A_Draw_Str("in good contact with pads.", &tft1, 8, 160, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 0);

				}  else {
					GC9A01A_fill(&tft1, BG_PRIMARY);
					GC9A01A_Draw_Str("Calculating...", &tft1, 40, 60, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 0);
									//else enough data, proceed with calculations!


					//clear
					memset(hrv_a, 0, sizeof(hrv_a));
					memset(hrv_b, 0, sizeof(hrv_b));

					sd_util_log_d("Player_A_PPG_RAW", player_a_ppg.data, player_a_ppg.idx);
					sd_util_log_d("Player_B_PPG_RAW", player_b_ppg.data, player_a_ppg.idx);

					data_processing_clean_ibi(player_a_ppg.data, player_a_ppg.idx);
					data_processing_clean_ibi(player_b_ppg.data, player_b_ppg.idx);

					sd_util_log_d("Player_A_PPG_CLEAN", player_a_ppg.data, player_a_ppg.idx);
					sd_util_log_d("Player_B_PPG_CLEAN", player_b_ppg.data, player_a_ppg.idx);

					data_processing_calc_1hz_hrv(player_a_ppg.data, player_a_ppg.idx, hrv_a, &hrv_a_n);
					data_processing_calc_1hz_hrv(player_b_ppg.data, player_b_ppg.idx, hrv_b, &hrv_b_n);


					//INSERT SAVE TO uSD CARD HERE FOR LOGGING... but to save ram, will now modify in place
					sd_util_log_f("Player_A_HRV_RAW", hrv_a, hrv_a_n);
					sd_util_log_f("Player_B_HRV_RAW", hrv_b, hrv_b_n);

					char msg_hrv_rg[25];
					sprintf(msg_hrv_rg, "Higher HRV R: %c", ('A' + (data_processing_calc_stdev(hrv_a, hrv_a_n, NULL) < data_processing_calc_stdev(hrv_b, hrv_b_n, NULL))));

					data_processing_normalize(hrv_a, NULL, hrv_a_n);
					data_processing_normalize(hrv_b, NULL, hrv_b_n);

					sd_util_log_f("Player_A_HRV_NORM", hrv_a, hrv_a_n);
					sd_util_log_f("Player_B_HRV_NORM", hrv_b, hrv_b_n);

					memset(gsr_a_normalized, 0, sizeof(gsr_a_normalized));
					memset(gsr_b_normalized, 0, sizeof(gsr_b_normalized));

					sd_util_log_d("Player_A_GSR_RAW", player_a_gsr.data, player_a_gsr.idx);
					sd_util_log_d("Player_B_GSR_RAW", player_b_gsr.data, player_b_gsr.idx);

					//cannot normalize these in place because of data type conversion to float
					data_processing_normalize_d(player_a_gsr.data, gsr_a_normalized, player_a_gsr.idx); // no need to subtract 1 because it is on the NEXT value, but since indices start at 0, that reflects how many have been written.
					data_processing_normalize_d(player_b_gsr.data, gsr_b_normalized, player_b_gsr.idx);

					sd_util_log_f("Player_A_GSR_NORM", gsr_a_normalized, player_a_gsr.idx);
					sd_util_log_f("Player_B_GSR_NORM", gsr_b_normalized, player_b_gsr.idx);

					//now everything is normalized, so do DTW to calculate scores!

					hrv_dtw = data_processing_DTW(hrv_a, hrv_b, hrv_a_n, hrv_b_n);
					gsr_dtw = data_processing_DTW(gsr_a_normalized, gsr_b_normalized, player_a_gsr.idx, player_b_gsr.idx);

					GC9A01A_fill(&tft1, BG_PRIMARY);

					char msg_gsr_rg[25];

					sprintf(msg_gsr_rg, "Higher GSR R: %c", ('A' + (data_processing_calc_stdev_d(player_a_gsr.data, player_a_gsr.idx, NULL) < data_processing_calc_stdev_d(player_b_gsr.data, player_b_gsr.idx, NULL))));

					char msg_hrv_dtw[25];
					char msg_gsr_dtw[25];

					sprintf(msg_hrv_dtw, "HRV DTW SCORE: %#.4g", hrv_dtw);
					sprintf(msg_gsr_dtw, "GSR DTW SCORE: %#.4g", gsr_dtw);

					GC9A01A_Draw_Str(msg_hrv_rg, &tft1, 40, 60, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 0);
					GC9A01A_Draw_Str(msg_gsr_rg, &tft1, 40, 80, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 0);
					GC9A01A_Draw_Str(msg_hrv_dtw, &tft1, 8, 100, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 0);
					GC9A01A_Draw_Str(msg_gsr_dtw, &tft1, 8, 120, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 0);

					char msg_pct_score[25];
					sprintf(msg_pct_score, "Match: %d %%", data_processing_combined_score(hrv_dtw, gsr_dtw));
					GC9A01A_Draw_Str(msg_pct_score, &tft1, 40, 140, u8g2_font_logisoso16_tr, TEXT_PRIMARY, BG_PRIMARY, 0);

					char final_score_formatted[125];

					sprintf(final_score_formatted, "%s\n%s\n%s\n%s\n%s", msg_hrv_rg, msg_gsr_rg, msg_hrv_dtw, msg_gsr_dtw, msg_pct_score);
					sd_util_log_str("finalscores", final_score_formatted);

				}

				state_updated = false;
				break;
			} else {
				//shouldn't be here ngl. Throwing error message above.
				if (l_pressed || c_pressed || r_pressed) {
					printf("button pressed\r\n");
					clearButtons();
					state_updated = true;
					current_app_state = APP_SPLASH;
				}
			}
	}
}

app_state_t app_states_get_state() {
	return current_app_state;
}


