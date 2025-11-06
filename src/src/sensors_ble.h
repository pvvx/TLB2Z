#ifndef _SENSOR_BLE_H_
#define _SENSOR_BLE_H_

#if (SENSOR_TYPE == SENSOR_BLE)

typedef struct _measured_data_t {
	u16	battery_mv; // mV
	u16	average_battery_mv; // mV
	s16	temp; // in 0.01 C
	s16	humi; // in 0.01 %
	u16	battery_level; // in 0.5%
	u16 count;
	u8  batVal; // 0..100%
	u8  flag;
} measured_data_t;

//extern u8 sensor_i2c_addr;
//extern u8 sensor_version;
extern measured_data_t measured_data;

#define sensor_go_sleep()
#define init_sensor()
#define read_sensor()	1

#endif // (SENSOR_TYPE == SENSOR_BLE)
#endif // _SENSOR_BLE_H_
