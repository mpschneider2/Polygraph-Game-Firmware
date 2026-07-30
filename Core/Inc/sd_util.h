/*
 * sd_util.h
 *
 *  Created on: Jul 26, 2026
 *      Author: matthewschneider
 */

#ifndef INC_SD_UTIL_H_
#define INC_SD_UTIL_H_

#include "fatfs.h"

void sd_util_init();
void generateFileIncre(FIL * Fil, char * inname, char * outname);
void sd_util_log_f(char * fname_pre, float * data, uint16_t data_n);
void sd_util_log_d(char * fname_pre, uint16_t * data, uint16_t data_n);
void sd_util_log_str(char * fname_pre, char * str);

#endif /* INC_SD_UTIL_H_ */
