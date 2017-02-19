/*
* MemLCD.c
*
* Created: 2017-01-03 오후 10:03:50
*  Author: Albireo
*/
#include "MemLCD.h"
#include "font.h"
#include "sprite.h"
#define SLAVE_SELECT_PIN  CONF_PIN_SPI_SSN

const uint8_t BLACK   = 0x00;
const uint8_t WHITE   = 0xE0;
const uint8_t RED	  = 0x80;
const uint8_t GREEN   = 0x40;
const uint8_t YELLOW  = 0xC0;
const uint8_t BLUE	  = 0x20;
const uint8_t MAGENTA = 0xA0;
const uint8_t CYAN	  = 0x60;

static void draw_pixel(uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH], uint8_t x, uint8_t y, uint8_t color) // Write single pixel to frameBuffer
{
	uint8_t bit_offset = (x*3)&0x07;
	frameBuffer[y][(x*3)>>3] = (frameBuffer[y][(x*3)>>3] & ~(0xE0>>bit_offset)) | (color>>bit_offset);
	if (bit_offset > 5)
	{
		frameBuffer[y][((x*3)>>3)+1] = (frameBuffer[y][((x*3)>>3)+1] & (0xFF>>(bit_offset-5))) | (color << (8-bit_offset));
	}
}

static void draw_char(char c,uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH], uint8_t x, uint8_t y, uint8_t font_color, uint8_t bg_color,const FONT_INFO* font) // Write single character to frameBuffer
{
	for (uint8_t line_count = 0 ; (line_count < font->height) & ((y + line_count) < FRAME_HEIGHT); line_count++)
	{
		if (font->charInfo[c - font->startChar].widthBits < 9)
		{
			for (uint8_t bit_offset = 0 ; (bit_offset < font->charInfo[c - font->startChar].widthBits) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + bit_offset, y + line_count, bg_color);
			}
		} 
		else if(font->charInfo[c - font->startChar].widthBits < 17)
		{
			for (uint8_t bit_offset = 0 ; (bit_offset < 8) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*2] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + bit_offset, y + line_count, bg_color);
			}
			for (uint8_t bit_offset = 0 ; (bit_offset < font->charInfo[c - font->startChar].widthBits - 8) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*2 + 1] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + 8 + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + 8 + bit_offset, y + line_count, bg_color);
			}
		}
		else if(font->charInfo[c - font->startChar].widthBits < 25)
		{
			for (uint8_t bit_offset = 0 ; (bit_offset < 8) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*3] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + bit_offset, y + line_count, bg_color);
			}
			for (uint8_t bit_offset = 0 ; (bit_offset < 16) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*3 + 1] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + 8 + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + 8 + bit_offset, y + line_count, bg_color);
			}
			for (uint8_t bit_offset = 0 ; (bit_offset < font->charInfo[c - font->startChar].widthBits - 16) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*3 + 2] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + 16 + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + 16 + bit_offset, y + line_count, bg_color);
			}
		}
		else if(font->charInfo[c - font->startChar].widthBits < 33)
		{
			for (uint8_t bit_offset = 0 ; (bit_offset < 8) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*4] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + bit_offset, y + line_count, bg_color);
			}
			for (uint8_t bit_offset = 0 ; (bit_offset < 16) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*4 + 1] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + 8 + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + 8 + bit_offset, y + line_count, bg_color);
			}
			for (uint8_t bit_offset = 0 ; (bit_offset < 24) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*4 + 2] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + 16 + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + 16 + bit_offset, y + line_count, bg_color);
			}
			for (uint8_t bit_offset = 0 ; (bit_offset < font->charInfo[c - font->startChar].widthBits - 24) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*4 + 3] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + 24 + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + 24 + bit_offset, y + line_count, bg_color);
			}
		}
		else if(font->charInfo[c - font->startChar].widthBits < 41)
		{
			for (uint8_t bit_offset = 0 ; (bit_offset < 8) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*5] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + bit_offset, y + line_count, bg_color);
			}
			for (uint8_t bit_offset = 0 ; (bit_offset < 16) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*5 + 1] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + 8 + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + 8 + bit_offset, y + line_count, bg_color);
			}
			for (uint8_t bit_offset = 0 ; (bit_offset < 24) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*5 + 2] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + 16 + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + 16 + bit_offset, y + line_count, bg_color);
			}
			for (uint8_t bit_offset = 0 ; (bit_offset < 32) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*5 + 3] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + 24 + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + 24 + bit_offset, y + line_count, bg_color);
			}
			for (uint8_t bit_offset = 0 ; (bit_offset < font->charInfo[c - font->startChar].widthBits - 32) & ((x + bit_offset) < 128) ; bit_offset++)
			{
				if(font->data[font->charInfo[c - font->startChar].offset + line_count*5 + 4] & (0x80>>bit_offset)) draw_pixel(frameBuffer, x + 32 + bit_offset, y + line_count, font_color);
				else draw_pixel(frameBuffer, x + 32 + bit_offset, y + line_count, bg_color);
			}
		}
	}
}

void draw_string(char* string, uint8_t length, uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH], uint8_t x, uint8_t y, uint8_t font_color, uint8_t bg_color,const FONT_INFO* font)
{
	for(uint8_t cursor = x, i = 0 ; i < length ; i++)
	{
		draw_char(string[i], frameBuffer, cursor, y, font_color, bg_color, font);
		cursor += font->charInfo[string[i] - font->startChar].widthBits;
		if(cursor < 127)
		{
			for(uint8_t line_count = 0 ; (line_count < font->height) & ((y + line_count) < FRAME_HEIGHT) & (i < (length - 1)) ; line_count++)
			{
				draw_pixel(frameBuffer, cursor, y + line_count, bg_color);
			}
			cursor++;
		}
		else return;
	}
}

void draw_horizontal_line(uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH], uint8_t x1, uint8_t x2, uint8_t y, uint8_t color)
{
	for(uint8_t i = x1; i <= x2; i++)
	{
		draw_pixel(frameBuffer, i, y, color);
	}
}

void draw_sprite(uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH], uint8_t x, uint8_t y, uint8_t color, uint8_t bg_color, SPRITE_INFO sprite)
{
	for(uint8_t line_count = y; (line_count < (y+sprite.height_pixels)) & (line_count < FRAME_HEIGHT) ; line_count++)
	{
		for(uint8_t byte_count = 0; byte_count < (sprite.width_bytes-1); byte_count++)
		{
			for(uint8_t bit_offset = 0; bit_offset < 8; bit_offset++)
			{
				if (sprite.bitmaps[(line_count-y)*sprite.width_bytes + byte_count] & (0x80 >> bit_offset)) draw_pixel(frameBuffer, x + byte_count*8 + bit_offset, line_count, color);
				else draw_pixel(frameBuffer, x + byte_count*8 + bit_offset, line_count, bg_color);
			}
		}
		for(uint8_t bit_offset = 0; bit_offset < (sprite.width_pixels & 0x07); bit_offset++)
		{
			if (sprite.bitmaps[(line_count-y)*sprite.width_bytes + (sprite.width_bytes-1)] & (0x80 >> bit_offset)) draw_pixel(frameBuffer, x + (sprite.width_bytes-1)*8 + bit_offset, line_count, color);
			else draw_pixel(frameBuffer, x + (sprite.width_bytes-1)*8 + bit_offset, line_count, bg_color);
		}
	}
}

static void delay_us(uint32_t time)
{
	dualtimer_set_counter(DUALTIMER_TIMER1,DUALTIMER_SET_CURRUNT_REG, time*26);
	while(dualtimer_get_value(DUALTIMER_TIMER1)){}
}

//! [configure_spi]
static void configure_spi_master(LCDSPIModule* module)
{
	//! [config]
	struct spi_config config_spi_master;
	//! [config]
	//! [slave_config]
	struct spi_slave_inst_config slave_dev_config;
	//! [slave_config]
	/* Configure and initialize software device instance of peripheral slave */
	//! [slave_conf_defaults]
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	//! [slave_conf_defaults]
	//! [ss_pin]
	slave_dev_config.ss_pin = SLAVE_SELECT_PIN;
	//! [ss_pin]
	//! [slave_init]
	spi_attach_slave(&(module->LCD_slave), &slave_dev_config);
	//! [slave_init]
	/* Configure, initialize and enable SPI module */
	//! [conf_defaults]
	spi_get_config_defaults(&config_spi_master);
	//! [conf_defaults]
	//! [transfer_mode]
	config_spi_master.transfer_mode = CONF_SPI_TRANSFER_MODE;
	//! [transfer_mode]
	//! [clock_divider]
	config_spi_master.clock_divider = 154;
	//! [clock_divider]
	/* Configure pad 0 */
	//! [sck]
	config_spi_master.pin_number_pad[0] = CONF_SPI_PIN_SCK;
	config_spi_master.pinmux_sel_pad[0] = CONF_SPI_MUX_SCK;
	//! [sck]
	/* Configure pad 1 */
	//! [mosi]
	config_spi_master.pin_number_pad[1] = CONF_SPI_PIN_MOSI;
	config_spi_master.pinmux_sel_pad[1] = CONF_SPI_MUX_MOSI;
	//! [mosi]
	/* Configure pad 2 */
	//! [ssn]
	config_spi_master.pin_number_pad[2] = PINMUX_UNUSED;
	config_spi_master.pinmux_sel_pad[2] = PINMUX_UNUSED;
	//! [ssn]
	/* Configure pad 3 */
	//! [miso]
	config_spi_master.pin_number_pad[3] = CONF_SPI_PIN_MISO;
	config_spi_master.pinmux_sel_pad[3] = CONF_SPI_MUX_MISO;
	//! [miso]
	//! [init]
	spi_init(&(module->LCD_spi_master_instance), CONF_SPI, &config_spi_master);
	//! [init]

	//! [enable]
	spi_enable(&(module->LCD_spi_master_instance));
	//! [enable]
}

void Setup(LCDSPIModule* module)
{
	configure_spi_master(module);

}

static uint8_t swap(uint8_t foo)
{
	uint8_t res = 0;
	res = res | ((foo & 0x01) << 7);
	res = res | ((foo & 0x02) << 5);
	res = res | ((foo & 0x04) << 3);
	res = res | ((foo & 0x08) << 1);
	res = res | ((foo & 0x10) >> 1);
	res = res | ((foo & 0x20) >> 3);
	res = res | ((foo & 0x40) >> 5);
	res = res | ((foo & 0x80) >> 7);
	return res;
}

void generate_color_test_pattern(uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH])
{
	for(uint8_t i = 0; i<128; i++)
	{
		for(uint8_t j=0; j<16;j++)
		{
			draw_pixel(frameBuffer, j, i, BLACK);
		}
		for(uint8_t j=16; j<32;j++)
		{
			draw_pixel(frameBuffer, j, i, RED);
		}
		for(uint8_t j=32; j<48;j++)
		{
			draw_pixel(frameBuffer, j, i, GREEN);
		}
		for(uint8_t j=48; j<64;j++)
		{
			draw_pixel(frameBuffer, j, i, YELLOW);
		}
		for(uint8_t j=64; j<80;j++)
		{
			draw_pixel(frameBuffer, j, i, BLUE);
		}
		for(uint8_t j=80; j<96;j++)
		{
			draw_pixel(frameBuffer, j, i, MAGENTA);
		}
		for(uint8_t j=96; j<112;j++)
		{
			draw_pixel(frameBuffer, j, i, CYAN);
		}
		for(uint8_t j=112; j<128;j++)
		{
			draw_pixel(frameBuffer, j, i, WHITE);
		}
	}
}

void Transfer(uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH], uint8_t startLine, uint8_t endLine, LCDSPIModule* module)
{
	//generate_color_test_pattern(frameBuffer);

	//! [select_slave]
	gpio_pin_set_output_level(PIN_LP_GPIO_16, true);
	//! [select_slave]

	//! [delay]
	delay_us(5); //tsSCS
	//! [delay]

	//! [write]
	spi_write(&(module->LCD_spi_master_instance), 0x80);// M0=1 M1=0 M2=0 DMY=0

	for(uint8_t line_count = startLine - 1; line_count < endLine; line_count++)
	{
		spi_write(&(module->LCD_spi_master_instance), swap(line_count + 1)); //Gate line address transfer
		spi_write_buffer_wait(&(module->LCD_spi_master_instance), frameBuffer[line_count], FRAME_WIDTH); //Data transfer
		spi_write(&(module->LCD_spi_master_instance), 0); // Dummy
	}

	spi_write(&(module->LCD_spi_master_instance), 0); // Dummy
	//! [write]
	//! [delay]
	delay_us(220);
	//! [delay]
	//! [deselect_slave]
	gpio_pin_set_output_level(PIN_LP_GPIO_16, false);
	//! [deselect_slave]
	delay_us(100);
}

void Display(LCDSPIModule* module)
{
	//! [select_slave]
	//spi_select_slave(&(module->LCD_spi_master_instance), &(module->LCD_slave), false);
	gpio_pin_set_output_level(PIN_LP_GPIO_16, true);
	//! [select_slave]

	//! [delay]
	delay_us(5);
	//! [delay]

	//! [write]
	spi_write(&(module->LCD_spi_master_instance), 0);// M0=0 M1=0 M2=0 DMY=0
	spi_write(&(module->LCD_spi_master_instance), 0); // Dummy
	//! [write]
	//! [delay]
	delay_us(170);
	//! [delay]
	//! [deselect_slave]
	//spi_select_slave(&(module->LCD_spi_master_instance), &(module->LCD_slave), true);
	gpio_pin_set_output_level(PIN_LP_GPIO_16, false);
	//! [deselect_slave]
	delay_us(1000);
}

void ClearDisplay(LCDSPIModule* module)
{
	//! [select_slave]
	//spi_select_slave(&(module->LCD_spi_master_instance), &(module->LCD_slave), false);
	gpio_pin_set_output_level(PIN_LP_GPIO_16, true);
	//! [select_slave]

	//! [delay]
	delay_us(5);
	//! [delay]

	//! [write]
	spi_write(&(module->LCD_spi_master_instance), 0x20);// M0=0 M1=0 M2=1 DMY=0
	spi_write(&(module->LCD_spi_master_instance), 0); // Dummy
	//! [write]
	//! [delay]
	delay_us(170);
	//! [delay]
	//! [deselect_slave]
	//spi_select_slave(&(module->LCD_spi_master_instance), &(module->LCD_slave), true);
	gpio_pin_set_output_level(PIN_LP_GPIO_16, false);
	//! [deselect_slave]
	delay_us(1000);
}

void ClearBuffer(uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH])
{
	for(uint8_t i=0;i<FRAME_HEIGHT;i++)
		for(uint8_t j=0;j<FRAME_WIDTH;j++)
			frameBuffer[i][j] = 0xFF;
}

void InvertBuffer(uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH])
{
	for(uint8_t i=0;i<FRAME_HEIGHT;i++)
		for(uint8_t j=0;j<FRAME_WIDTH;j++)
			frameBuffer[i][j] = ~frameBuffer[i][j];
}