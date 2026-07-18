/*
 * lcd.h
 *
 * Created: 2026-07-01 오후 2:31:28
 *  Author: kccistc
 */ 
#ifndef LCD_H_
#define LCD_H_

#define F_CPU 16000000UL  // 16MHz
#include <avr/io.h>  // PORTA PORTB PORTD... IO관련 reg가 들어 있다.
#include <util/delay.h>  // _delay_ms _delay_us 등
#include "i2c.h"

#define PORT_DATA		PORTF               // 데이터 핀 연결 포트
#define PORT_CONTROL	PORTC			  // 제어 핀 연결 포트
#define DDR_DATA		DDRF			      // 데이터 핀의 데이터 방향
#define DDR_CONTROL		DDRC			  // 제어 핀의 데이터 방향

#define RS_PIN 0					  // RS 제어 핀의 비트 번호
#define RW_PIN 1					  // R/W 제어 핀의 비트 번호
#define E_PIN  2					  // E 제어 핀의 비트 번호

#define COMMAND_CLEAR_DISPLAY 0x01
#define COMMAND_CURSOR_RETURN 0x02
#define COMMAND_8_BIT_MODE    0x38
#define COMMAND_4_BIT_MODE    0x28

#define COMMAND_DISPLAY_ON_OFF_BIT 2
#define COMMAND_CURSOR_ON_OFF_BIT  1
#define COMMAND_BLINK_ON_OFF_BIT   0

#define COMMAND_DISPLAY_LR_BIT 1
#define COMMAND_DISPLAY_SHIFT_BIT 0

extern uint8_t MODE;

void lcd_pulse_enable(void);
void lcd_write_data(uint8_t data);
void lcd_write_command(uint8_t command);
void lcd_clear(void);
void lcd_init(void);
void lcd_rtc_mode(void);
void lcd_set_time(void);
void lcd_multi_calc(void);
void lcd_remove_data(int idx);
void lcd_write_string(char *string);
void lcd_goto_XY(uint8_t row, uint8_t col);
void lcd_display_rtc(t_ds1307* ds1307);
void lcd_display_clc(char* pbuff);
void shift_cursor_left(void);
void shift_cursor_right(void);
void shift_display_left(void);
void shift_display_right(void);

#endif