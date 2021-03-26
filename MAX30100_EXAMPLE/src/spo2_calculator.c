/*
 * spo2_calculator.c
 *
 * Created: 25/03/2021 20:54:30
 *  Author: eduardo
 */ 
#include "spo2_calculator.h"
#include <asf.h>
#include <fastmath.h>


const uint8_t spO2LUT[43] = {100,100,100,100,99,99,99,99,99,99,98,98,98,98,
	98,97,97,97,97,97,97,96,96,96,96,96,96,95,95,
95,95,95,95,94,94,94,94,94,93,93,93,93,93};

float irACValueSqSum;
float redACValueSqSum;
uint8_t beatsDetectedNum;
uint32_t samplesRecorded;
uint8_t spO2;

void spo2_calculator_reset() {
	irACValueSqSum = 0;
	redACValueSqSum = 0;
	beatsDetectedNum = 0;
	samplesRecorded = 0;
	spO2 = 0;
}

void spo2_calculator_update(float irACValue, float redACValue, uint32_t beatDetected) {
	irACValueSqSum += irACValue * irACValue;
	redACValueSqSum += redACValue * redACValue;
	++samplesRecorded;

	if (beatDetected) {
		++beatsDetectedNum;
		if (beatsDetectedNum == CALCULATE_EVERY_N_BEATS) {
			float acSqRatio = 100.0 * log(redACValueSqSum/samplesRecorded) / log(irACValueSqSum/samplesRecorded);
			uint8_t index = 0;

			if (acSqRatio > 66) {
				index = (uint8_t)acSqRatio - 66;
				} else if (acSqRatio > 50) {
				index = (uint8_t)acSqRatio - 50;
			}
			spo2_calculator_reset();

			spO2 = spO2LUT[index];
		}
	}
}

uint8_t spo2_calculator_get() {
	return spO2;
}
