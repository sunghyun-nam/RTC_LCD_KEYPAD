/*
 * mode.h
 *
 * Created: 2026-07-02 오후 11:41:16
 *  Author: tjdgu
 */ 

#define F_CPU 16000000UL   // CPU clock 설정은 무조건 맨 앞에 할 것 (중요)
#include <avr/io.h>
#include "i2c.h"

void rtc_clock_mode(t_ds1307* ds1307, t_ds1307* ds1307_bk);
void rtc_set_mode(t_ds1307* ds1307, t_ds1307* ds1307_bk);
void calc_mode(char *buff);

typedef enum
{
	MODE_RTC_CLOCK,
	MODE_RTC_SET,
	MODE_CALC
} t_mode;

typedef struct
{
	uint8_t btn0_sub;
	uint8_t btn1_add;
	
	uint8_t target_idx;
	uint8_t	btn2_idx;
	uint8_t btn2_order[6];
	// idx 0 -> year (0 ~ 99)  | idx 1 -> month (1 ~ 12)   | idx 2 -> date (1 ~ 31)
	// idx 3 -> hours (0 ~ 23) | idx 4 -> minutes (0 ~ 59) | idx 5 -> seconds (0 ~ 59)
	uint8_t min_val[6];
	uint8_t max_val[6];
	uint8_t btn2_pos[6][2];
	
	uint8_t btn3_cmd_display;
	uint8_t btn3_set_time;
	
	uint8_t btn4_cmd_display;
} t_rtc_ui;