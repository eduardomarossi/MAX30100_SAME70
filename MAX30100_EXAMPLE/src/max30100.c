/*
 * max30100.c
 *
 * Created: 25/03/2021 19:27:49
 *  Author: eduardo
 */ 
#include <asf.h>
#include "max30100.h"
#include "fifo.h"

fifo_desc_t max30100_ir_fifo;
uint16_t max30100_ir_fifo_buffer[16];
fifo_desc_t max30100_red_fifo;
uint16_t max30100_red_fifo_buffer[16];

void max30100_write_reg(uint8_t addr, uint8_t *buffer, uint32_t bufferlen) {
	twihs_packet_t packet_tx;

	/* Configure the data packet to be transmitted */
	packet_tx.chip        = MAX30100_CHIP_ID;
	packet_tx.addr[0]     = addr;
	packet_tx.addr_length = 1;
	packet_tx.buffer      = (uint8_t *) buffer;
	packet_tx.length      = bufferlen;

	if (twihs_master_write(MAX30100_I2C, &packet_tx) != TWIHS_SUCCESS) {
		puts("-E-\tTWI master write packet failed.\r");
		while (1);
	}
	
	delay_ms(WAIT_TIME);
}

void max30100_write_reg8(uint8_t addr, uint8_t value) {
	twihs_packet_t packet_tx;

	/* Configure the data packet to be transmitted */
	packet_tx.chip        = MAX30100_CHIP_ID;
	packet_tx.addr[0]     = addr;
	packet_tx.addr_length = 1;
	packet_tx.buffer      = &value;
	packet_tx.length      = 1;

	if (twihs_master_write(MAX30100_I2C, &packet_tx) != TWIHS_SUCCESS) {
		puts("-E-\tTWI master write packet failed.\r");
		while (1) {
			
		}
	}
	
	delay_ms(WAIT_TIME);
}

void max30100_read_reg(uint8_t addr, uint8_t *buffer, uint32_t buffersize) {
	twihs_packet_t packet_rx;

	/* Configure the data packet to be received */
	packet_rx.chip        = MAX30100_CHIP_ID;
	packet_rx.addr[0]     = addr;
	packet_rx.addr_length = 1;
	packet_rx.buffer      = buffer;
	packet_rx.length      = buffersize;
	
	
	/* Get memory from EEPROM*/
	uint32_t ret = twihs_master_read(MAX30100_I2C, &packet_rx);
	
	if (ret != TWIHS_SUCCESS) {
		puts("-E-\tTWI master read packet failed.\r");
		while (1) {
			/* Capture error */
		}
	}
	delay_ms(WAIT_TIME);
}


uint8_t max30100_read_reg8(uint8_t addr) {
	uint8_t data;
	
	max30100_read_reg(addr, &data, 1);
	return data;
}


uint16_t max30100_read_reg16(uint8_t addr) {
	uint16_t data;
	
	max30100_read_reg(addr, &data, 2);
	return data;
}

float max30100_read_temperature() {
	uint8_t v;
	v = max30100_read_reg8(MAX30100_MODECFG_REG);

	max30100_write_reg8(MAX30100_MODECFG_REG, v | MAX30100_MODECFG_TEMPEN);

	while (max30100_read_reg8(MAX30100_MODECFG_REG) & MAX30100_MODECFG_TEMPEN) {
		delay_ms(1);
	}

	uint16_t tempreading = max30100_read_reg16(MAX30100_TEMPDATAINT_REG);
	uint8_t tfrac = tempreading & 0xF;
	int8_t tint = tempreading >> 8;

	float temp = tint;
	temp += tfrac * 0.0625;
	return temp;
}


void max30100_init() {
	twihs_options_t opt;
	
	/* Configure the options of TWI driver */
	opt.master_clk = sysclk_get_peripheral_hz();
	opt.speed      = TWIHS_CLK;
	opt.smbus = 0;
	
	/* Enable the peripheral clock for TWI */
	pmc_enable_periph_clk(MAX30100_I2C_ID);
	
	if (twihs_master_init(MAX30100_I2C, &opt) != TWIHS_SUCCESS) {
		puts("-E-\tTWI master initialization failed.\r");
		while (1) {
			/* Capture error */
		}
	}
	
	if(max30100_read_reg8(MAX30100_REG_PARTID) != MAX30100_EXPECTED_PARTID) {
		puts("-E-\tRead wrong part ID from MAX30100.\r");
		while(1){}
	}
	// Config
	max30100_write_reg8(MAX30100_LEDCFG_REG, 0x8F);
	max30100_write_reg8(MAX30100_MODECFG_REG, 0x03);
	max30100_write_reg8(MAX30100_SPO2CFG_REG, 0x47);
		 
	// Reset Buffer
	max30100_write_reg8(MAX30100_FIFO_WRPTR_REG, 0x00);
	max30100_write_reg8(MAX30100_FIFO_OFLOW_REG, 0x00);
	max30100_write_reg8(MAX30100_FIFO_RDPTR_REG, 0x00);
	
	fifo_init(&max30100_ir_fifo, &max30100_ir_fifo_buffer[0], sizeof(max30100_ir_fifo_buffer)/sizeof(uint16_t));
	fifo_init(&max30100_red_fifo, &max30100_red_fifo_buffer[0], sizeof(max30100_red_fifo_buffer)/sizeof(uint16_t));
}


void max30100_read_fifo_data()
{
	uint8_t num_sample = 16; //(wpointer - rpointer) & (MAX30100_FIFO_DEPTH-1);
	uint8_t fifo_data[64] = { 0 };
	uint8_t reg = MAX30100_FIFO_DATA_REG;
	uint8_t writeData[] = { reg };
	
	if(num_sample) {
		max30100_read_reg(reg, &fifo_data[0], num_sample * 4);
		for(uint32_t i = 0; i < num_sample; i++) {
			uint16_t ir = (uint16_t)((fifo_data[i*4] << 8) | fifo_data[i*4 + 1]);
			uint16_t red = (uint16_t)((fifo_data[i*4 + 2] << 8) | fifo_data[i*4 + 3]);
			
			if(fifo_is_full(&max30100_ir_fifo)) {
				fifo_pull_uint16_nocheck(&max30100_ir_fifo);
				fifo_pull_uint16_nocheck(&max30100_red_fifo);
			}
			fifo_push_uint16_nocheck(&max30100_ir_fifo, ir);
			fifo_push_uint16_nocheck(&max30100_red_fifo, red);
		}
	}
}

uint32_t max30100_get_raw_values(uint16_t *ir, uint16_t *red) {
	if(!fifo_is_empty(&max30100_ir_fifo)) {
		*ir = fifo_pull_uint32_nocheck(&max30100_ir_fifo);
		*red = fifo_pull_uint32_nocheck(&max30100_red_fifo);
		return 1;
	} else {
		return 0;
	}
}

void max30100_set_leds_current(LEDCurrent irLedCurrent, LEDCurrent redLedCurrent)
{
	max30100_write_reg8(MAX30100_REG_LED_CONFIGURATION, redLedCurrent << 4 | irLedCurrent);
}
