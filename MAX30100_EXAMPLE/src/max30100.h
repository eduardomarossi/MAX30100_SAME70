/*
 * max30100.h
 *
 * Created: 04/02/2021 21:04:17
 *  Author: Eduardo
 */ 


#ifndef MAX30100_H_
#define MAX30100_H_
#include <asf.h>

/** I2C Wait Time */
#define WAIT_TIME   10

/** TWI Bus Clock 100kHz */
#define TWIHS_CLK  400000

#define MAX30100_I2C      TWIHS0
#define MAX30100_I2C_ID   ID_TWIHS0

#define MAX30100_ADDRESS           0x50
#define MAX30100_ADDRESS_LENGTH    0
#define MAX30100_CHIP_ID  0x57

#define MAX30100_REG_PARTID 0xFF
#define MAX30100_EXPECTED_PARTID 0x11

#define MAX30100_TEMPDATAINT_REG         0x16
#define MAX30100_TEMPDATAFRAC_REG        0x17

#define MAX30100_IRQSTAT_REG             0x00
#define MAX30100_IRQSTAT_PWRREADY        0x01
#define MAX30100_IRQSTAT_SPO2READY       0x10
#define MAX30100_IRQSTAT_HRREADY         0x20
#define MAX30100_IRQSTAT_TEMPREADY       0x40
#define MAX30100_IRQSTAT_AFULL           0x80

#define MAX30100_IRQENABLE_REG           0x01
#define MAX30100_IRQENABLE_SPO2READY     0x10
#define MAX30100_IRQENABLE_HRREADY       0x20
#define MAX30100_IRQENABLE_TEMPREADY     0x40
#define MAX30100_IRQENABLE_AFULL         0x80

#define MAX30100_FIFO_WRPTR_REG          0x02
#define MAX30100_FIFO_OFLOW_REG          0x03
#define MAX30100_FIFO_RDPTR_REG          0x04
#define MAX30100_FIFO_DATA_REG           0x05

#define MAX30100_MODECFG_REG             0x06
#define MAX30100_MODECFG_HRONLY          0x02
#define MAX30100_MODECFG_HRSP02          0x03
#define MAX30100_MODECFG_TEMPEN          0x08
#define MAX30100_MODECFG_RESET           0x40
#define MAX30100_MODECFG_SHDN            0x80

#define MAX30100_SPO2CFG_REG             0x07
#define MAX30100_SPO2CFG_HIGHRES         0x40

#define MAX30100_LEDCFG_REG              0x09
#define MAX30100_FIFO_DEPTH  0x10

#define MAX30100_REG_LED_CONFIGURATION          0x09
typedef enum LEDCurrent {
	MAX30100_LED_CURR_0MA      = 0x00,
	MAX30100_LED_CURR_4_4MA    = 0x01,
	MAX30100_LED_CURR_7_6MA    = 0x02,
	MAX30100_LED_CURR_11MA     = 0x03,
	MAX30100_LED_CURR_14_2MA   = 0x04,
	MAX30100_LED_CURR_17_4MA   = 0x05,
	MAX30100_LED_CURR_20_8MA   = 0x06,
	MAX30100_LED_CURR_24MA     = 0x07,
	MAX30100_LED_CURR_27_1MA   = 0x08,
	MAX30100_LED_CURR_30_6MA   = 0x09,
	MAX30100_LED_CURR_33_8MA   = 0x0a,
	MAX30100_LED_CURR_37MA     = 0x0b,
	MAX30100_LED_CURR_40_2MA   = 0x0c,
	MAX30100_LED_CURR_43_6MA   = 0x0d,
	MAX30100_LED_CURR_46_8MA   = 0x0e,
	MAX30100_LED_CURR_50MA     = 0x0f
} LEDCurrent;



void max30100_write_reg(uint8_t addr, uint8_t *buffer, uint32_t bufferlen);
void max30100_write_reg8(uint8_t addr, uint8_t value);
void max30100_read_reg(uint8_t addr, uint8_t *buffer, uint32_t buffersize);
uint8_t max30100_read_reg8(uint8_t addr);
uint16_t max30100_read_reg16(uint8_t addr);
float max30100_read_temperature();
void max30100_init();
void max30100_read_fifo_data();
uint32_t max30100_get_raw_values(uint16_t *ir, uint16_t *red);
void max30100_set_leds_current(LEDCurrent irLedCurrent, LEDCurrent redLedCurrent);

#endif