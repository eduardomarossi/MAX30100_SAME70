/*
 * pulse_oximeter.h
 *
 * Created: 25/03/2021 19:39:46
 *  Author: eduardo
 */ 


#ifndef PULSE_OXIMETER_H_
#define PULSE_OXIMETER_H_


typedef struct {
	float v[2];
} butter_filter;

typedef struct {
	float alpha;
	float dcw;
} dc_remover_filter;


#define PULSEOXIMETER_STATE_INIT 1
#define PULSEOXIMETER_STATE_IDLE 2
#define PULSEOXIMETER_STATE_DETECTING 3
#define SAMPLING_FREQUENCY                  100
#define CURRENT_ADJUSTMENT_PERIOD_MS        500
#define DEFAULT_IR_LED_CURRENT              MAX30100_LED_CURR_50MA
#define RED_LED_CURRENT_START               MAX30100_LED_CURR_27_1MA
#define DC_REMOVER_ALPHA                    0.95

float dc_remover_step(dc_remover_filter* filter, float x);
float butter_filter_step(butter_filter* filter, float x);
float pulse_oximeter_get_heart_rate();
uint32_t pulse_oximeter_begin();
void pulse_oximeter_set_callback(void (*cb)());
uint8_t pulse_oximeter_get_spo2();
void pulse_oximeter_update();
void pulse_oximeter_init();

#endif /* PULSE_OXIMETER_H_ */