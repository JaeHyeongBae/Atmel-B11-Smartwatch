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
#include "MemLCD.h"
#include "platform.h"
#include "console_serial.h"
#include "at_ble_api.h"
#include "ble_manager.h"
#include "csc_app.h"
#include "cscp.h"
#include "cscs.h"
#include "dualtimer.h"
#include "font.h"

#define APP_STACK_SIZE  (1024)

volatile unsigned char app_stack_patch[APP_STACK_SIZE];

/* Received notification data structure */
csc_report_ntf_t recv_ntf_info;

/* Data length to be send over the air */
uint16_t send_length = 0;

/* Buffer data to be send over the air */
uint8_t send_data[APP_TX_BUF_SIZE];

LCDSPIModule module;
uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH];

uint8_t hour = 10;
uint8_t minute = 49;
uint8_t ds = 0;

uint8_t month = 2;
uint8_t date = 14;
uint8_t day = 2;

bool media_playing = true;

bool update_watchface_flag = false;
bool draw_flag = false;

static const char *str_day[7] =
{
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
	"Sun"
};

static const char *str_month[12] =
{
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

static const ble_event_callback_t app_gap_handle[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	app_connected_event_handler,
	app_disconnected_event_handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static void uart_rx_callback(uint8_t input)
{
	if(input == '\r') {
		if(send_length) {
			send_plf_int_msg_ind(UART_RX_COMPLETE, UART_RX_INTERRUPT_MASK_RX_FIFO_NOT_EMPTY_MASK, send_data, send_length);
			memset(send_data, 0, APP_TX_BUF_SIZE);
			send_length = 0;
			DBG_LOG(" ");
		}
	}
	else {
		send_data[send_length++] = input;
		DBG_LOG_CONT("%c", input);
		
		if(send_length >= APP_TX_BUF_SIZE) {
			send_plf_int_msg_ind(UART_RX_COMPLETE, UART_RX_INTERRUPT_MASK_RX_FIFO_NOT_EMPTY_MASK, send_data, send_length);
			memset(send_data, 0, APP_TX_BUF_SIZE);
			send_length = 0;
		}
	}
}

/**
* @brief app_connected_state blemanager notifies the application about state
* @param[in] at_ble_connected_t
*/
static at_ble_status_t app_connected_event_handler(void *params)
{
	return AT_BLE_SUCCESS;
}

/**
 * @brief app_connected_state ble manager notifies the application about state
 * @param[in] connected
 */
static at_ble_status_t app_disconnected_event_handler(void *params)
{
		/* Started advertisement */
		csc_prf_dev_adv();		

		return AT_BLE_SUCCESS;
}

/* Function used for receive data */
static void csc_app_recv_buf(uint8_t *recv_data, uint8_t recv_len)
{
	uint16_t ind = 0;
	if (recv_len){
		for (ind = 0; ind < recv_len; ind++){
			DBG_LOG_CONT("%c", recv_data[ind]);
		}
		DBG_LOG("\r\n");
	}
	draw_string(recv_data, recv_len, frameBuffer, 1, 1, BLACK, WHITE, msSans8pt);
	draw_flag = true;
	switch(recv_data[0])
	{
		case 't' :
		hour = recv_data[1];
		minute = recv_data[2];
		break;
	}
}

/* Callback called for new data from remote device */
static void csc_prf_report_ntf_cb(csc_report_ntf_t *report_info)
{
	DBG_LOG("\r\n");
	csc_app_recv_buf(report_info->recv_buff, report_info->recv_buff_len);
}

/* Function used for send data */
static void csc_app_send_buf(void)
{
	uint16_t plf_event_type;
	uint16_t plf_event_data_len;
	uint8_t plf_event_data[APP_TX_BUF_SIZE] = {0, };

	platform_event_get(&plf_event_type, plf_event_data, &plf_event_data_len);
	
	if(plf_event_type == ((UART_RX_INTERRUPT_MASK_RX_FIFO_NOT_EMPTY_MASK << 8) | UART_RX_COMPLETE)) {
		csc_prf_send_data(plf_event_data, plf_event_data_len);
	}

}

static void ao_gpio_callback(void)
{
	gpio_pin_toggle_output_level(LED_0_PIN);
}

static void configure_gpio_pins(void)
{
	struct gpio_config config_gpio_pin;
	gpio_get_config_defaults(&config_gpio_pin);
	config_gpio_pin.direction = GPIO_PIN_DIR_OUTPUT;
	gpio_pin_set_config(PIN_LP_GPIO_16 , &config_gpio_pin);
	gpio_pin_set_config(PIN_LP_GPIO_20 , &config_gpio_pin);
	gpio_pin_set_config(LED_0_PIN , &config_gpio_pin);
	gpio_pin_set_output_level(PIN_LP_GPIO_16, false);
	gpio_pin_set_output_level(PIN_LP_GPIO_20, false);
	gpio_pin_set_output_level(LED_0_PIN, false);

	struct gpio_config config_ao_gpio_pin;
	gpio_get_config_defaults(&config_ao_gpio_pin);
	config_ao_gpio_pin.direction = GPIO_PIN_DIR_INPUT;
	config_ao_gpio_pin.input_pull = GPIO_PIN_PULL_NONE;
	config_ao_gpio_pin.aon_wakeup = true;
	gpio_pin_set_config(PIN_AO_GPIO_2,&config_ao_gpio_pin);
}

static void configure_gpio_callbalck( void)
{
	gpio_init();
	gpio_register_callback(PIN_AO_GPIO_2, ao_gpio_callback, GPIO_CALLBACK_RISING);
	gpio_enable_callback(PIN_AO_GPIO_2);
	//NVIC_EnableIRQ(GPIO1_IRQn);
	
}

static void dualtimer_callback2(void)
{
	gpio_pin_toggle_output_level(PIN_LP_GPIO_20);
}

static void configure_dualtimer_callback(void)
{
	dualtimer_register_callback(DUALTIMER_TIMER2, dualtimer_callback2);
	NVIC_EnableIRQ(DUALTIMER0_IRQn);
}


static void configure_dualtimer(void)
{
	struct dualtimer_config config_dualtimer;
	dualtimer_get_config_defaults(&config_dualtimer);
	config_dualtimer.timer1.load_value = 0;
	config_dualtimer.timer2.load_value = 2600000;
	config_dualtimer.timer1.clock_prescaler = DUALTIMER_CLOCK_PRESCALER_DIV1;
	config_dualtimer.timer1.counter_mode = DUALTIMER_ONE_SHOT_MODE;
	config_dualtimer.timer2.clock_prescaler = DUALTIMER_CLOCK_PRESCALER_DIV1;
	config_dualtimer.timer2.counter_mode = DUALTIMER_PERIODIC_MODE;
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

static void csc_init()
{
	platform_driver_init();
	acquire_sleep_lock();

	/* Initialize serial console  */
	serial_console_init();

	DBG_LOG("Initializing Custom Serial Chat Application");
	
	/* Initialize the buffer address and buffer length based on user input */
	csc_prf_buf_init(&send_data[0], APP_TX_BUF_SIZE);
	
	/* initialize the ble chip  and Set the device mac address */
	ble_device_init(NULL);
	
	/* Initializing the profile */
	csc_prf_init(NULL);
	
	/* Started advertisement */
	csc_prf_dev_adv();
	
	ble_mgr_events_callback_handler(REGISTER_CALL_BACK,
	BLE_GAP_EVENT_TYPE,
	app_gap_handle);
	
	/* Register the notification handler */
	notify_recv_ntf_handler(csc_prf_report_ntf_cb);
	
	/* Register the user event handler */
	register_ble_user_event_cb(csc_app_send_buf);
	
	register_uart_callback(uart_rx_callback);
}

static uint8_t get_center_align_x(char* str, uint8_t len, const FONT_INFO *font)
{
	uint8_t x = 0;
	for(uint8_t i = 0; i < len; i++)
	{
		x += font->charInfo[str[i] - font->startChar].widthBits;
		x += 1;
	}
	x = 64 - (x >> 1);
	return x;
}

static void update_watchface( void)
{
	ClearBuffer(frameBuffer);
	char str_time[7];
	if(minute > 10) sprintf(str_time, "%d : %d", hour, minute);
	else sprintf(str_time, "%d : 0%d", hour, minute);
	draw_string(str_time, strlen(str_time), frameBuffer, get_center_align_x(str_time, strlen(str_time), josefinSans48pt), 28, BLACK, WHITE, josefinSans48pt);
	char str_date[11];
	sprintf(str_date, "%s, %d %s", str_day[day], date, str_month[month]);
	draw_string(str_date, strlen(str_date), frameBuffer, get_center_align_x(str_date, strlen(str_date),josefinSans16pt), 1, BLACK, WHITE, josefinSans16pt);
	char *song_name = "Overtime - Cash Cash";
	draw_horizontal_line(frameBuffer, 8, 120, 75, BLACK);
	draw_string(song_name, strlen(song_name), frameBuffer, get_center_align_x(song_name, strlen(song_name),msSans8pt), 78, BLACK, WHITE, msSans8pt);
	draw_horizontal_line(frameBuffer, 8, 120, 94, BLACK);
	draw_sprite(frameBuffer, 12, 96, BLACK, WHITE, missed_call);
	//InvertBuffer(frameBuffer);

	Transfer(frameBuffer,1,128,&module);
	update_watchface_flag = false;
}
static void aon_sleep_timer_callback(void)
{
	//gpio_pin_toggle_output_level(LED_0_PIN);
	update_watchface_flag = true;
	if(minute<59) minute++;
	else
	{
		minute = 0;
		if(hour<23) hour++;
		else hour = 0;
	}
	update_watchface();
}
static void configure_aon_sleep_timer(void)
{
	//! [setup_6]
	struct aon_sleep_timer_config config_aon_sleep_timer;
	//! [setup_6]
	//! [setup_7]
	aon_sleep_timer_get_config_defaults(&config_aon_sleep_timer);
	//! [setup_7]
	//! [setup_8]
	config_aon_sleep_timer.counter = 1966080; //1 min
	config_aon_sleep_timer.mode = AON_SLEEP_TIMER_RELOAD_MODE;
	config_aon_sleep_timer.wakeup = AON_SLEEP_TIMER_WAKEUP_ARM_BLE;
	//! [setup_8]
	//! [setup_9]
	aon_sleep_timer_init(&config_aon_sleep_timer);
	//! [setup_9]

}

static void configure_aon_sleep_timer_callback(void)
{
	//! [setup_register_callback]
	aon_sleep_timer_register_callback(aon_sleep_timer_callback);
	//! [setup_register_callback]

	//! [enable_IRQ]
	NVIC_EnableIRQ(AON_SLEEP_TIMER0_IRQn);
	//! [enable_IRQ]
}



bool app_exec = true;
int main( void)
{
    /* Initialize the SAM system */
    //system_clock_config(CLOCK_RESOURCE_XO_26_MHZ, CLOCK_FREQ_26_MHZ);
	csc_init();
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	configure_aon_sleep_timer();
	configure_aon_sleep_timer_callback();

	configure_gpio_pins();
	configure_gpio_callbalck();
	configure_dualtimer();
	configure_dualtimer_callback();
	asm volatile ("wfi");
	asm volatile ("nop");

	Setup(&module);
	ClearDisplay(&module);
	ClearDisplay(&module);
	ClearDisplay(&module);
	ClearBuffer(frameBuffer);
	update_watchface();


	while(app_exec){
		ble_event_task(BLE_EVENT_TIMEOUT);
		if(update_watchface_flag) update_watchface();
		if(draw_flag) Transfer(frameBuffer, 1, 128, &module); draw_flag = false;
	}
	return 0;
}
