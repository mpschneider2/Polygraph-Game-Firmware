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

app_state_t current_app_state = APP_RECORDING;


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

	GC9A01A_fill(&tft1, color[color_idx]);
	u8g2_SetBackgroundColor(&font, GC9A01A_OLIVE);
	u8g2_DrawStr(&font, 65, 170, "78");

}

void app_states_run() {
	switch (current_app_state) {
		case APP_RECORDING:

			adc_driver_resume_adc(); // start it if not started
			//HANDLE DATA ACQUISITION


			//HANDLE BUTTON PRESSES

			break;
	}
}

app_state_t app_states_get_state() {
	return current_app_state;
}


