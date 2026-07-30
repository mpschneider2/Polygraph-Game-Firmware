/*
 * sd_util.c
 *
 *  Created on: Jul 26, 2026
 *      Author: matthewschneider
 */

#include "sd_util.h"
#include "main.h"
#include "fatfs.h"
#include <stdio.h>

extern SD_HandleTypeDef hsd1;

static FATFS FatFs;
FRESULT FR_Status;

//void sd_util_init() {
//	  hsd1.Instance = SDMMC1;
//	  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
//	  hsd1.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
//	  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_ENABLE;
//	  hsd1.Init.BusWide = SDMMC_BUS_WIDE_1B;
//	  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
//	  hsd1.Init.ClockDiv = 2;
//	  HAL_SD_Init(&hsd1);
//}

//INIT CODE DEFINED IN MAIN.C

void sd_util_init() {
	FR_Status = f_mount(&FatFs, SDPath, 1);

	if (FR_Status != FR_OK) {
		printf("Unable to Mount Card. Status: %d\r\n", FR_Status);
		return;
	}

}

void generateFileIncre(FIL * Fil, char * inname, char * outname) {
	//assume both have adequate space

	FILINFO fno;
	FRESULT fr;

	int i = 0;
	for (i = 0; i < 999; i++) {
		sprintf(outname, "%s_%03d.txt", inname, i);
		fr = f_stat(outname, &fno);
		if (fr == FR_OK) {
			continue;
		} else {
			printf("Error creating incremented filename.\r\n");
			break;
		}
	}
	return;
}


void sd_util_log_f(char * fname_pre, float * data, uint16_t data_n) {

	FIL Fil;
	UINT RWC, WWC;
	DWORD FreeClusters;
	uint32_t TotalSize, FreeSpace;
	static char RW_Buffer[25];

	char fname[30];
	generateFileIncre(&Fil, fname_pre, fname);

	FR_Status = f_open(&Fil, fname, FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
	if (FR_Status != FR_OK) {
		printf("Unable to create new file. Error: %d\r\n", FR_Status);
		return;
	}

	for (uint16_t i = 0; i < data_n; i++) {
		sprintf(RW_Buffer, "%.6f ", data[i]);
		f_puts(RW_Buffer, &Fil);
	}

	f_close(&Fil);
}

void sd_util_log_d(char * fname_pre, uint16_t * data, uint16_t data_n) {

	FIL Fil;
	UINT RWC, WWC;
	DWORD FreeClusters;
	uint32_t TotalSize, FreeSpace;
	static char RW_Buffer[25];

	char fname[25];
	generateFileIncre(&Fil, fname_pre, fname);

	FR_Status = f_open(&Fil, fname, FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
	if (FR_Status != FR_OK) {
		printf("Unable to create new file. Error: %d\r\n", FR_Status);
		return;
	}

	for (uint16_t i = 0; i < data_n; i++) {
		sprintf(RW_Buffer, "%d ", data[i]);
		f_puts(RW_Buffer, &Fil);
	}

	f_close(&Fil);
}

void sd_util_log_str(char * fname_pre, char * str) {
	FIL Fil;
	UINT RWC, WWC;
	DWORD FreeClusters;
	uint32_t TotalSize, FreeSpace;
	static char RW_Buffer[25];

	char fname[25];
	generateFileIncre(&Fil, fname_pre, fname);

	FR_Status = f_open(&Fil, fname, FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
	if (FR_Status != FR_OK) {
		printf("Unable to create new file. Error: %d\r\n", FR_Status);
		return;
	}

	f_puts(str, &Fil);

	f_close(&Fil);
}

