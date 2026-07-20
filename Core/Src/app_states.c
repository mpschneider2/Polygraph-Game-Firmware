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

app_state_t current_app_state = APP_RECORDING;

ppg_instance_t player_a_ppg;
ppg_instance_t player_b_ppg;

extern SPI_HandleTypeDef hspi1;

GC9A01A tft1;
u8g2_font_t font;

void app_states_init() {
	current_app_state = APP_RECORDING;

	printf("Initializing display \r\n");

	GC9A01A_init(&tft1, &hspi1,
	  	  LCD_CS_GPIO_Port, LCD_CS_Pin,
	  	  LCD_DC_GPIO_Port, LCD_DC_Pin,
	  	  LCD_BLK_GPIO_Port, LCD_BLK_Pin,
	  	  LCD_RESET_GPIO_Port, LCD_RESET_Pin
	    );

	memset(&font, 0, sizeof(font));
	u8g2_SetDisplay(&font, &tft1);
	u8g2_SetFont(&font, u8g2_font_logisoso92_tn); // CHANGE!!
	u8g2_SetForegroundColor(&font, 0x0000); // black
	u8g2_SetBackgroundColor(&font, 0xFFFF); // white
	u8g2_SetFontMode(&font, 0); //faster to be nontransparent WITHOUT framebuffer
	u8g2_SetFontDirection(&font, 0);

	GC9A01A_fill(&tft1, GC9A01A_OLIVE);
	u8g2_SetBackgroundColor(&font, GC9A01A_OLIVE);
	u8g2_DrawStr(&font, 65, 170, "78");

	adc_driver_init(&player_a_ppg, &player_b_ppg);
	ppg_processor_init(&player_a_ppg);
	ppg_processor_init(&player_b_ppg);

	adc_driver_resume_adc(); // start it if not started
	ppg_processor_start(&player_a_ppg);
	ppg_processor_start(&player_b_ppg);

}

void app_states_run() {
	switch (current_app_state) {
		case APP_RECORDING:

			//HANDLE DATA ACQUISITION
			//ALL READS ARE DONE BY INTERRUPTS!
			if (player_a_ppg.new_data) {
				//use sample size of 5
				__disable_irq();

				if (player_a_ppg.idx > 4) {
					uint32_t sum = 0;
					for (int i = 0; i<5; i++) {
						sum += player_a_ppg.data[player_a_ppg.idx-i-1]; //idx points to NEXT slot
					}
					uint8_t bpm = 5.0f/(float)sum*1000.0f/60.0f;
					player_a_ppg.new_data = false;

					__enable_irq();

					printf("Beat.\r\n");
					char bpm_formatted[4];
					sprintf(bpm_formatted, "%d", bpm);
					u8g2_DrawStr(&font, 65, 170, bpm_formatted);
				}
			}

			//HANDLE BUTTON PRESSES

			break;
	}
}

app_state_t app_states_get_state() {
	return current_app_state;
}


