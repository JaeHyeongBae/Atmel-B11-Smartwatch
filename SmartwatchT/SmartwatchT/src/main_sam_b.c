/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * This is a bare minimum user application template.
 *
 * For documentation of the board, go \ref group_common_boards "here" for a link
 * to the board-specific documentation.
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# Basic usage of on-board LED and button
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

/*
  Since we do not properly generate system init code, we define DONT_USE_CMSIS_INIT
  before including the device headers. We also comment out the call to SystemInit().
*/


//#define DONT_USE_CMSIS_INIT

#include <asf.h>
#include "MemoryLCD/MemLCD.h"

LCDSPIModule module;
uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH];

static void timer_callback(void)
{
	gpio_pin_toggle_output_level(PIN_LP_GPIO_20);
}

static void configure_timer(void)
{
	struct timer_config config_timer;
	timer_get_config_defaults(&config_timer);
	config_timer.reload_value = 2600000;
	timer_init(&config_timer);
	timer_enable();
}
 static void configure_timer_callback(void)
 {
	 timer_register_callback(timer_callback);
	 
	 NVIC_EnableIRQ(TIMER0_IRQn);
 }

static void configure_gpio_pins(void)
{
	struct gpio_config config_gpio_pin;
	gpio_get_config_defaults(&config_gpio_pin);
	config_gpio_pin.direction = GPIO_PIN_DIR_OUTPUT;
	gpio_pin_set_config(PIN_LP_GPIO_16 , &config_gpio_pin);
	gpio_pin_set_config(PIN_LP_GPIO_20 , &config_gpio_pin);
	gpio_pin_set_output_level(PIN_LP_GPIO_16, false);
	gpio_pin_set_output_level(PIN_LP_GPIO_20, false);
}


static void configure_dualtimer(void)
{
	struct dualtimer_config config_dualtimer;
	dualtimer_get_config_defaults(&config_dualtimer);
	config_dualtimer.timer1.load_value = 0;
	config_dualtimer.timer2.load_value = 0;
	config_dualtimer.timer1.clock_prescaler = DUALTIMER_CLOCK_PRESCALER_DIV1;
	config_dualtimer.timer1.counter_mode = DUALTIMER_ONE_SHOT_MODE;
	dualtimer_init(&config_dualtimer);
}

static void delay_us(uint32_t time)
{
	dualtimer_set_counter(DUALTIMER_TIMER1,DUALTIMER_SET_CURRUNT_REG, time*26);
	while(dualtimer_get_value(DUALTIMER_TIMER1)){}
}

static void generate_test_pattern()
{
	for(uint8_t i=0;i<32;i++)
	{
		for(uint8_t j=0;j<24;j++)
		{
			frameBuffer[i][j] = 0xFF;
		}
		for(uint8_t j=24;j<48;j++)
		{
			frameBuffer[i][j] = 0x00;
		}
	}
	for(uint8_t i=32;i<64;i++)
	{
		for(uint8_t j=0;j<24;j++)
		{
			frameBuffer[i][j] = 0x00;
		}
		for(uint8_t j=24;j<48;j++)
		{
			frameBuffer[i][j] = 0xFF;
		}
	}
	for(uint8_t i=64;i<96;i++)
	{
		for(uint8_t j=0;j<24;j++)
		{
			frameBuffer[i][j] = 0xFF;
		}
		for(uint8_t j=24;j<48;j++)
		{
			frameBuffer[i][j] = 0x00;
		}
	}
	for(uint8_t i=96;i<128;i++)
	{
		for(uint8_t j=0;j<24;j++)
		{
			frameBuffer[i][j] = 0x00;
		}
		for(uint8_t j=24;j<48;j++)
		{
			frameBuffer[i][j] = 0xFF;
		}
	}
}

int main(void)
{
    /* Initialize the SAM system */
    system_clock_config(CLOCK_RESOURCE_XO_26_MHZ, CLOCK_FREQ_26_MHZ);
	configure_timer();
	configure_timer_callback();
	configure_gpio_pins();
	configure_dualtimer();
	Setup(&module);
	

	//delay_us(100000);
	//ClearDisplay(&module);
	//Transfer(frameBuffer,0,50,&module);
	//Display(&module);
	ClearBuffer(frameBuffer);
	//ClearDisplay(&module);
	while(1){
		ClearDisplay(&module);
		delay_us(1000000);
		Transfer(frameBuffer,1,128,&module);
		delay_us(1000000);

	}
}
