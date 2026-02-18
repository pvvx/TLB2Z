/********************************************************************************************************
 * @file    drv_adc.c
 *
 * @brief   This is the source file for drv_adc
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/

#include "../tl_common.h"

#if	defined(MCU_CORE_B91) || defined(MCU_CORE_TL321X) || defined(MCU_CORE_TL721X)
	#define ADC_SAMPLE_NUM						1	//fixed
	#define ADC_SAMPLE_FREQ						ADC_SAMPLE_FREQ_96K
	#define ADC_SAMPLE_NDMA_DELAY_TIME			((1000 / ( 6 * (2 << (ADC_SAMPLE_FREQ)))) + 1)//delay 2 sample cycle
#if defined(MCU_CORE_TL721X)
	#define ADC_PRESCALE						ADC_PRESCALE_1F8
#else//b91/b92/tl321x
	#define ADC_PRESCALE						ADC_PRESCALE_1F4
#endif
#endif

#if defined(MCU_CORE_TL321X) || defined(MCU_CORE_TL721X)
/**
 * @brief This function serves to get adc sample code by manual and convert to voltage value.
 * @return 		adc_vol_mv_average 	- the average value of adc voltage value.
 */
static u16 adc_get_voltage(void)
{
	u16 adc_sample_data = 0;

	/* get adc sample data */
	adc_start_sample_nodma();

	while (1)
	{
		u8 sample_cnt = adc_get_rxfifo_cnt();
		if (sample_cnt > 0)
		{
			adc_sample_data= adc_get_code();
			break;
		}
	}
	adc_stop_sample_nodma();

	if(adc_sample_data & BIT(11)){
		//12 bit resolution, BIT(11) is sign bit, 1 means negative voltage in differential_mode
		adc_sample_data = 0;
	}else{
		//BIT(10..0) is valid adc code
		adc_sample_data &= 0x7FF;
	}

	return adc_calculate_voltage(ADC_M_CHANNEL, adc_sample_data);
}
#endif

/****
* brief: ADC initiate function
* param[in] null
*
* @return	  1: set success ;
*             0: set error
*/
bool drv_adc_init(void)
{
#if	defined(MCU_CORE_8258)
	adc_init();
#elif defined(MCU_CORE_B91)

#elif defined(MCU_CORE_TL321X) || defined(MCU_CORE_TL721X)
	adc_init(NDMA_M_CHN);
#endif
	return TRUE;
}

/****
* brief: get the sample data
* param[in] null
* @return,the result
*/
u16 drv_get_adc_data(void)
{
#if defined(MCU_CORE_8258) || defined(MCU_CORE_B91)
	return (u16)adc_sample_and_get_result();
#elif defined(MCU_CORE_TL321X) || defined(MCU_CORE_TL721X)
	return (u16)adc_get_voltage();
#else
	return 0;
#endif
}

/****
* brief: Set ADC mode and pin
* param[in] mode, base or vbat mode
* param[in] pin, the pin number
* @return
*/
#if defined(MCU_CORE_8258)
void drv_adc_mode_pin_set(drv_adc_mode_e mode, GPIO_PinTypeDef pin)
{
	if(mode == DRV_ADC_BASE_MODE){
		adc_base_init(pin);
	}else if(mode == DRV_ADC_VBAT_MODE){
		adc_vbat_init(pin);
	}
}
#elif defined(MCU_CORE_B91)
void drv_adc_mode_pin_set(drv_adc_mode_e mode, adc_input_pin_def_e pin)
{
	/* DRV_ADC_BASE_MODE is recommended */
	if(mode == DRV_ADC_BASE_MODE){
		adc_gpio_sample_init(pin, ADC_VREF_1P2V, ADC_PRESCALE, ADC_SAMPLE_FREQ);
	}else if(mode == DRV_ADC_VBAT_MODE){
		/* The battery voltage sample range is 1.8~3.5V,
		 * and must set sys_init() function with the mode for battery voltage less than 3.6V.
		 * For battery voltage > 3.6V, should take some external voltage divider.
		 */
		(void)pin;
		adc_battery_voltage_sample_init();
	}
}
#elif defined(MCU_CORE_TL321X) || defined(MCU_CORE_TL721X)
void drv_adc_mode_pin_set(drv_adc_mode_e mode, adc_input_pin_e pin)
{
	if(mode == DRV_ADC_BASE_MODE){
		adc_gpio_cfg_t adc_gpio_cfg_m;
		adc_gpio_cfg_m.v_ref = ADC_VREF_1P2V;
		adc_gpio_cfg_m.pre_scale = ADC_PRESCALE;
		adc_gpio_cfg_m.sample_freq = ADC_SAMPLE_FREQ;
		adc_gpio_cfg_m.pin = pin;

		adc_gpio_sample_init(ADC_M_CHANNEL, adc_gpio_cfg_m);
	}else if(mode == DRV_ADC_VBAT_MODE){
		(void)pin;
		adc_vbat_sample_init(ADC_M_CHANNEL);
	}
}
#endif

/**
 * @brief      This function sets sar_adc power.
 * @param[in]  enable enable=1 : power on. enable=0: power off.
 * @return     none
 */
void drv_adc_enable(bool enable)
{
#if defined(MCU_CORE_8258)
	adc_power_on_sar_adc((unsigned char)enable);
#elif defined(MCU_CORE_B91) || defined(MCU_CORE_TL321X) || defined(MCU_CORE_TL721X)
	if(enable){
		adc_power_on();

#if defined(MCU_CORE_TL321X) || defined(MCU_CORE_TL721X)
		delay_us(30);//Wait >30us after adc_power_on() for ADC to be stable.
#endif
	}else{
		adc_power_off();
	}
#endif
}


