/*
 * lcd.c
 *
 * Created: 2026-07-01 오후 2:31:08
 *  Author: kccistc
 */ 

#include "lcd.h"

void lcd_pulse_enable(void)             // 하강 에지에서 동작
{
	PORT_CONTROL |= (1 << E_PIN);       // E -> HIGH
	_delay_ms(1);
	PORT_CONTROL &= ~(1 << E_PIN);		// E -> LOW
	_delay_ms(1);
}

void lcd_write_data(uint8_t data)
{
	PORT_CONTROL |= (1 << RS_PIN);		// 문자 출력 RS: 1
	if (MODE == 8)
	{
		PORT_DATA = data;
		lcd_pulse_enable();
	}
	else
	{
		PORT_DATA = data & 0xF0;		// 상위 4비트
		lcd_pulse_enable();
		PORT_DATA = (data << 4) & 0xF0; // 하위 4비트
		lcd_pulse_enable();
	}
	_delay_ms(2);
}

void lcd_write_command(uint8_t command)
{
	PORT_CONTROL &= ~(1 << RS_PIN);		// 명령어 실행 RS: 0
	if (MODE == 8)
	{
		PORT_DATA = command;
		lcd_pulse_enable();
	}
	else
	{
		PORT_DATA = command & 0xF0;		// 상위 4비트
		lcd_pulse_enable();
		PORT_DATA = (command << 4) & 0xF0; // 하위 4비트
		lcd_pulse_enable();
	}
	_delay_ms(2);
}

void lcd_clear(void)
{
	lcd_write_command(COMMAND_CLEAR_DISPLAY);
	_delay_ms(2);
}

void lcd_cursor_return(void)
{
	lcd_write_command(COMMAND_CURSOR_RETURN);
	_delay_ms(2);
}

void lcd_init(void)
{
	_delay_ms(50);			// 초기 구동 시간
	
	if (MODE == 8) DDR_DATA |= 0xFF;
	else DDR_DATA |= 0xF0;
	
	PORT_DATA = 0x00;
	
	DDR_CONTROL |= 1 << RS_PIN | 1 << RW_PIN | 1 << E_PIN;
	
	// R/W 핀으로 LOW를 출력하여 쓰기 전용으로 사용
	PORT_CONTROL &= ~(1 << RW_PIN);
	
	if (MODE == 8)
	{
		lcd_write_command(COMMAND_8_BIT_MODE);
	}
	else
	{
		lcd_write_command(0x02);
		lcd_write_command(COMMAND_4_BIT_MODE);
	}
	
	// Entry Mode Set
	// 출력 후 커서를 오른쪽으로 옮김
	lcd_write_command(0x06);
}

void lcd_rtc_mode(void)
{
	uint8_t command = 0x08 | (1 << COMMAND_DISPLAY_ON_OFF_BIT);
	lcd_write_command(command);
	
	lcd_clear();
}

void lcd_set_time(void)
{
	uint8_t command = 0x08 | (1 << COMMAND_DISPLAY_ON_OFF_BIT) | (1 << COMMAND_CURSOR_ON_OFF_BIT) | (1 << COMMAND_BLINK_ON_OFF_BIT);
	lcd_write_command(command);
	
	lcd_goto_XY(0, 2);
}

void lcd_multi_calc(void)
{
	uint8_t command = 0x08 | (1 << COMMAND_DISPLAY_ON_OFF_BIT) | (1 << COMMAND_CURSOR_ON_OFF_BIT) | (1 << COMMAND_BLINK_ON_OFF_BIT);
	lcd_write_command(command);
	
	lcd_clear();
}

void shift_cursor_left(void)
{
	lcd_write_command(0x10);
}

void shift_cursor_right(void)
{
	lcd_write_command(0x14);
}

void shift_display_left(void)
{
	lcd_write_command(0x18);
}

void shift_display_right(void)
{
	lcd_write_command(0x1C);
}

void lcd_remove_data(int idx)
{
	lcd_goto_XY(0, idx);
	lcd_write_data(' ');
}

void lcd_write_string(char *string)
{
	uint8_t i;
	for (i = 0; string[i]; i++)		// 종료 문자를 만날 때까지
	{
		lcd_write_data(string[i]);
	}
}

void lcd_goto_XY(uint8_t row, uint8_t col)
{
	//col %= 16;
	//row %= 2;
	
	// 첫째 라인 시작 주소는 0x00, 둘째 라인 시작 주소는 0x40
	uint8_t address = (0x40 * row) + col;
	uint8_t command = 0x80 + address;
	
	lcd_write_command(command);
}

void lcd_display_rtc(t_ds1307* ds1307)
{
	lcd_goto_XY(0, 0);
	lcd_write_string("20");
	lcd_write_data((ds1307->year / 10) + '0');
	lcd_write_data((ds1307->year % 10) + '0');
	lcd_write_data('-');
	lcd_write_data((ds1307->month / 10) + '0');
	lcd_write_data((ds1307->month % 10) + '0');
	lcd_write_data('-');
	lcd_write_data((ds1307->date / 10) + '0');
	lcd_write_data((ds1307->date % 10) + '0');
	
	lcd_goto_XY(1, 0);
	lcd_write_data((ds1307->hours / 10) + '0');
	lcd_write_data((ds1307->hours % 10) + '0');
	lcd_write_data(':');
	lcd_write_data((ds1307->minutes / 10) + '0');
	lcd_write_data((ds1307->minutes % 10) + '0');
	lcd_write_data(':');
	lcd_write_data((ds1307->seconds / 10) + '0');
	lcd_write_data((ds1307->seconds % 10) + '0');
}

void lcd_display_clc(char* pbuff)
{
	lcd_goto_XY(0, 0);
	for (int i = 0; pbuff[i]; i++)
	{
		lcd_write_data(pbuff[i]);	
	}
}