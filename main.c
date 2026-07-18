/*
 * 95.RTC_LCD.c
 *
 * Created: 2026-07-01 오후 8:34:51
 * Author : kccistc
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#include "cal.h"
#include "lcd.h"
#include "i2c.h"
#include "mode.h"
#include "button.h"
#include "keypad.h"

void init_timer0(void);

uint8_t MODE=4;
t_mode app_mode = MODE_RTC_CLOCK;

t_rtc_ui rtc_ui = {
	.btn0_sub = 0,
	.btn1_add = 0,
	.target_idx = 0,
	.btn2_idx = 0,

	.btn2_order = {6, 5, 4, 2, 1, 0},
	.min_val = {0, 1, 1, 0, 0, 0},
	.max_val = {99, 12, 31, 23, 59, 59},
	.btn2_pos = {
		{0, 2},
		{0, 5},
		{0, 8},
		{1, 0},
		{1, 3},
		{1, 6}
	},

	.btn3_cmd_display = 0,
	.btn3_set_time = 0,
	
	.btn4_cmd_display = 0
};

volatile uint32_t keypad_counter=0;
volatile uint32_t msec_count=0;
volatile uint32_t msec_btn=0;

ISR(TIMER0_OVF_vect)
{
	volatile uint8_t keydata=0;
	
	TCNT0 = 6;      // TCNT0 6~256 : 250개 펄스 count하기 위해
	msec_count++;
	msec_btn++;
	
	if (++keypad_counter >=60)
	{
		keypad_counter = 0;
		if (keydata=keypad_scan()) // keypad를 check해서 눌려 진것이 있으면
		{
			insert_queue(keydata);					   // circular queue에 저장 한다.
		}
	}
}


int main(void)
{
	t_ds1307 ds1307;
	t_ds1307 ds1307_bk;
	
	char buff[100]={0};
	
	lcd_init();
	init_date_time(&ds1307);
	i2c_init();
	init_ds1307(&ds1307);
	init_keypad();
	init_button();
	init_timer0();
	sei();
	
	
	while (1)
	{	
		switch(app_mode)
		{
			case MODE_RTC_CLOCK:
				rtc_clock_mode(&ds1307, &ds1307_bk);
				break;
			case MODE_RTC_SET:
				rtc_set_mode(&ds1307, &ds1307_bk);
				break;
			case MODE_CALC:
				calc_mode(buff);
				break;
		}
	}
}

void init_timer0(void)
{
	TCNT0 = 6;   // TCNT0 6~256 : 250개 펄스 count하기 위해
	TCCR0 &= ~(1 << CS02 | 1 << CS01 | 1 << CS00); // 0분주
	TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00;	   // 64분주
	TIMSK |= 1 << TOIE0;	// TIMER0 Overflow INT
}


