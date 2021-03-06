﻿/*
 * MemLCD.h
 *
 * Created: 2017-01-03 오후 10:45:00
 *  Author: Albireo
 */ 
 #pragma once

 #include <asf.h>
 #include "font.h"
 #include "sprite.h"

 #define FRAME_WIDTH	48 // size of bytes for a horizontal line of fame buffer
 #define FRAME_HEIGHT	128 // number of horizontal lines in a frame buffer

 typedef struct spi_module spi_master_instance;
 typedef struct spi_slave_inst slave;

 typedef struct {
	 spi_master_instance LCD_spi_master_instance;
	 slave LCD_slave;
 }LCDSPIModule;

extern const uint8_t BLACK;
extern const uint8_t WHITE;
extern const uint8_t RED;
extern const uint8_t GREEN;
extern const uint8_t YELLOW;
extern const uint8_t BLUE;
extern const uint8_t MAGENTA;
extern const uint8_t CYAN;

void Transfer (uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH], uint8_t startLine, uint8_t endLine, LCDSPIModule*); // transfers line (startLine) to line(endLine)(inclusive) to MemoryLCD
void Display(LCDSPIModule*);
void ClearDisplay(LCDSPIModule*);
void Enable (void);
void Disable (void);
void ClearBuffer(uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH]);
void InvertBuffer(uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH]);
void Setup(LCDSPIModule*);
void generate_color_test_pattern(uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH]);
void draw_string(char* string, uint8_t length, uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH], uint8_t x, uint8_t y, uint8_t font_color, uint8_t bg_color,const FONT_INFO* font);
void draw_horizontal_line(uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH], uint8_t x1, uint8_t x2, uint8_t y, uint8_t color);
void draw_sprite(uint8_t frameBuffer[FRAME_HEIGHT][FRAME_WIDTH], uint8_t x, uint8_t y, uint8_t color, uint8_t bg_color, SPRITE_INFO sprite);