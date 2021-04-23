#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "max30100.h"
#include "time_utils.h"
#include "pulse_oximeter.h"

static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
		#endif
		.paritytype = CONF_UART_PARITY,
		#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
		#endif
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

void on_beat() {
	printf("Beat!\r\n");
}

/**
* \brief Application entry point for TWI EEPROM example.
*
* \return Unused (ANSI-C compatibility).
*/
int main(void)
{
	sysclk_init();
	board_init();
	configure_console();
	systick_config();
	
	max30100_init();
	puts("MAX30100 detected!\r\n");
	pulse_oximeter_init();
	pulse_oximeter_begin();
	pulse_oximeter_set_callback(on_beat);
	uint16_t ir, red;
	uint32_t tsLastReport = 0;
	while (1) {
		max30100_read_fifo_data();
		pulse_oximeter_update();
		if(systick_get_counter() - tsLastReport > 1000) {
			printf("Heartrate: %f Spo2: %d\r\n", pulse_oximeter_get_heart_rate(), pulse_oximeter_get_spo2());
		}
	
	}
}
