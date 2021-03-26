/*
 * time_utils.h
 *
 * Created: 25/03/2021 20:34:33
 *  Author: eduardo
 */ 


#ifndef TIME_UTILS_H_
#define TIME_UTILS_H_

/** Global timestamp in milliseconds since start of application */
uint32_t systick_get_counter(void);
void systick_config(void);

void SysTick_Handler(void);

#endif /* TIME_UTILS_H_ */