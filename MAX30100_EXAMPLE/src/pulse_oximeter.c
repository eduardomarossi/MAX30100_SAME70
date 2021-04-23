/*
 * pulse_oximeter.c
 *
 * Created: 25/03/2021 19:39:34
 *  Author: eduardo
 */ 

#include <asf.h>
#include "pulse_oximeter.h"
#include "beatdetector.h"
#include "max30100.h"
#include "spo2_calculator.h"
#include "time_utils.h"

uint32_t pulse_state;
uint32_t tsLastBiasCheck, tsFirstBeatDetected, tsLastBeatDetected, tsLastCurrentAdjustment;
LEDCurrent redLedCurrentIndex, irLedCurrent;
void (*onBeatDetected)() = NULL;
	

float dc_remover_step(dc_remover_filter* filter, float x) {
	float olddcw = filter->dcw;
	filter->dcw = (float)x + filter->alpha * filter->dcw;

	return filter->dcw - olddcw;
}

float butter_filter_step(butter_filter* filter, float x) {
	filter->v[0] = filter->v[1];
	filter->v[1] = (2.452372752527856026e-1 * x)
	+ (0.50952544949442879485 * filter->v[0]);
	return(filter->v[0] + filter->v[1]);
}

dc_remover_filter ir_dc_remover, red_dc_remover;
butter_filter lpf;
beatdetector beat;

#define DC_REMOVER_ALPHA 0.95

void pulse_oximeter_init() {
	ir_dc_remover.alpha = DC_REMOVER_ALPHA;
	ir_dc_remover.dcw = 0;
	red_dc_remover.dcw = 0;
	red_dc_remover.alpha = DC_REMOVER_ALPHA;
	lpf.v[0] = 0;
	lpf.v[1] = 0;
	beatdetector_init(&beat);
	pulse_state = PULSEOXIMETER_STATE_INIT;
	tsLastBiasCheck = 0;
	tsFirstBeatDetected = 0;
	tsLastBeatDetected = 0;
	tsLastCurrentAdjustment = 0;
	redLedCurrentIndex = (uint8_t)MAX30100_LED_CURR_27_1MA;
	irLedCurrent = MAX30100_LED_CURR_50MA;
}

uint32_t pulse_oximeter_begin() {
	max30100_set_leds_current(irLedCurrent, (LEDCurrent)redLedCurrentIndex);

	pulse_state = PULSEOXIMETER_STATE_IDLE;

	return 1;
}

void pulse_oximeter_check_sample() {
	uint16_t raw_ir, raw_red;

	while(max30100_get_raw_values(&raw_ir, &raw_red)) {
		float irACValue = dc_remover_step(&ir_dc_remover, raw_ir);
		float redACValue = dc_remover_step(&red_dc_remover, raw_red);
		// The signal fed to the beat detector is mirrored since the cleanest monotonic spike is below zero
		float filteredPulseValue = butter_filter_step(&lpf, -irACValue);
		uint32_t beatDetected = beatdetector_add_sample(&beat, filteredPulseValue);

		if (beatdetector_get_rate(&beat) > 0) {
			pulse_state = PULSEOXIMETER_STATE_DETECTING;
			spo2_calculator_update(irACValue, redACValue, beatDetected);
		} else if (pulse_state == PULSEOXIMETER_STATE_DETECTING) {
			pulse_state = PULSEOXIMETER_STATE_IDLE;
			spo2_calculator_reset();
		}

		if (beatDetected && onBeatDetected) {
			onBeatDetected();
		}
	}
}

void pulse_oximeter_check_current_bias() {
	// Follower that adjusts the red led current in order to have comparable DC baselines between
	// red and IR leds. The numbers are really magic: the less possible to avoid oscillations
	
	// Follower that adjusts the red led current in order to have comparable DC baselines between
	// red and IR leds. The numbers are really magic: the less possible to avoid oscillations
	
	if (systick_get_counter() - tsLastBiasCheck > CURRENT_ADJUSTMENT_PERIOD_MS) {
		uint32_t changed = 0;
		if (ir_dc_remover.dcw - red_dc_remover.dcw > 70000 && redLedCurrentIndex < MAX30100_LED_CURR_50MA) {
			++redLedCurrentIndex;
			changed = 1;
		} else if (red_dc_remover.dcw - ir_dc_remover.dcw > 70000 && redLedCurrentIndex > 0) {
			--redLedCurrentIndex;
			changed = 1;
		}

		if (changed) {
			max30100_set_leds_current(irLedCurrent, (LEDCurrent)redLedCurrentIndex);
			tsLastCurrentAdjustment = systick_get_counter();
			
			printf("I: %d\r\n", redLedCurrentIndex);
		}

		tsLastBiasCheck = systick_get_counter();
	}
}

void pulse_oximeter_update() {
	max30100_read_fifo_data();
	pulse_oximeter_check_sample();
	pulse_oximeter_check_current_bias();
}

uint8_t pulse_oximeter_get_spo2() {
	return spo2_calculator_get();
}

float pulse_oximeter_get_heart_rate() {
	return beatdetector_get_rate(&beat);
}

void pulse_oximeter_set_callback(void (*cb)()) {
	onBeatDetected = cb;
}
