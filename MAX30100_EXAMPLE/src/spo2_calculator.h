/*
 * spo2_calculator.h
 *
 * Created: 25/03/2021 20:55:04
 *  Author: eduardo
 */ 


#ifndef SPO2_CALCULATOR_H_
#define SPO2_CALCULATOR_H_
#define CALCULATE_EVERY_N_BEATS         3
#include <asf.h>

void spo2_calculator_reset();
void spo2_calculator_update(float irACValue, float redACValue, uint32_t beatDetected);
uint8_t spo2_calculator_get();
void spo2_calculator_reset();
void spo2_calculator_update(float irACValue, float redACValue, uint32_t beatDetected) ;
uint8_t spo2_calculator_get();

#endif /* SPO2_CALCULATOR_H_ */