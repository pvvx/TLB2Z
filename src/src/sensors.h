/*
 * sensor.h
 *
 *  Created on: 14 нояб. 2023 г.
 *      Author: pvvx
 */

#ifndef _SENSORS_H_
#define _SENSORS_H_

void adc_channel_init(ADC_InputPchTypeDef p_ain); // in adc_drv.c
u16 get_adc_mv(int flg); // in adc_drv.c
void battery_detect(void);

#endif /* _SENSORS_H_ */
