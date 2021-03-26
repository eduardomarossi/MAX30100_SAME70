/*
 * time_utils.c
 *
 * Created: 25/03/2021 20:39:21
 *  Author: eduardo
 */ 

#include <asf.h>
#include "time_utils.h"

volatile uint32_t g_ul_ms_ticks = 0;

void SysTick_Handler(void)
{
	g_ul_ms_ticks++;
}

void systick_config(void) {
	if (SysTick_Config(sysclk_get_cpu_hz() / 1000)) {
		puts("-E- Systick configuration error\r");
		while (1) {
			/* Capture error */
		}
	}
}


uint32_t systick_get_counter(void) {
	return g_ul_ms_ticks;
}