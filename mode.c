/*
 * mode.c
 *
 * Created: 2026-07-02 오후 11:41:01
 *  Author: tjdgu
 */ 


#define F_CPU 16000000UL   // CPU clock 설정은 무조건 맨 앞에 할 것 (중요)
#include <avr/io.h>
#include "lcd.h"
#include "cal.h"
#include "i2c.h"
#include "mode.h"
#include "queue.h"
#include "button.h"

extern volatile uint32_t msec_count;
extern t_mode app_mode;
extern t_rtc_ui rtc_ui;

void rtc_clock_mode(t_ds1307* ds1307, t_ds1307* ds1307_bk)
{	
	if (get_button(BUTTON3, BUTTON3PIN))
	{
		rtc_ui.btn2_idx = 0;
		rtc_ui.btn3_cmd_display = 1;
		
		app_mode = MODE_RTC_SET;	// RTC MODE -> SET TIME MODE
		return;  // 모드 변경이 일어난 직후, 해당 함수를 즉시 벗어남 
	}
	else if (get_button(BUTTON4, BUTTON4PIN))
	{
		rtc_ui.btn4_cmd_display = 1;
		
		app_mode = MODE_CALC;	// RTC MODE -> CALC MODE
		return; // 모드 변경이 일어난 직후, 해당 함수를 즉시 벗어남
	}
	
	if (rtc_ui.btn3_cmd_display)
	{
		lcd_rtc_mode();		// DISPLAY ON | CURSOR OFF | BLINK OFF
		rtc_ui.btn3_cmd_display = 0;
		
		if (!is_equal(ds1307, ds1307_bk))  // SET TIME MODE에서 설정한 값이 RTC MODE -> SET TIME MODE로
		{									 // 진입하기 전 BACKUP한 값과 다르면 RTC 값 재설정
			init_ds1307(ds1307);
		}
	}
	
	if (rtc_ui.btn4_cmd_display)
	{
		lcd_rtc_mode();
		rtc_ui.btn4_cmd_display = 0;
	}
	
	if (msec_count >= 1000)
	{
		msec_count = 0;
		
		read_ds1307(ds1307);
		lcd_display_rtc(ds1307);
	}
}

void rtc_set_mode(t_ds1307* ds1307, t_ds1307* ds1307_bk)
{	
	uint8_t* p = (uint8_t*)ds1307;
	
	if (get_button(BUTTON3, BUTTON3PIN))
	{
		rtc_ui.btn2_idx = 0;
		rtc_ui.btn3_cmd_display = 1;
		
		app_mode = MODE_RTC_CLOCK;	// SET TIME MODE -> RTC MODE
		return;
	}
	
	if (get_button(BUTTON0, BUTTON0PIN))
	{
		rtc_ui.btn0_sub = 1;	 // 값 감소
	}
	else if (get_button(BUTTON1, BUTTON1PIN))
	{
		rtc_ui.btn1_add = 1;	 // 값 증가
	}
	else if (get_button(BUTTON2, BUTTON2PIN))
	{
		rtc_ui.btn2_idx = (rtc_ui.btn2_idx + 1) % 6;	// SET TIME MODE에서만 동작
	}
	
	if (rtc_ui.btn3_cmd_display)
	{
		lcd_set_time();		// DISPLAY ON | CURSOR ON | BLINK ON
		rtc_ui.btn3_cmd_display = 0;
		
		*ds1307_bk = *ds1307;
	}
	
	lcd_goto_XY(rtc_ui.btn2_pos[rtc_ui.btn2_idx][0], rtc_ui.btn2_pos[rtc_ui.btn2_idx][1]);
	if (rtc_ui.btn0_sub || rtc_ui.btn1_add)
	{
		rtc_ui.target_idx = rtc_ui.btn2_order[rtc_ui.btn2_idx];
		if (rtc_ui.btn0_sub)
		{
			if (p[rtc_ui.target_idx] <= rtc_ui.min_val[rtc_ui.btn2_idx])		// 최솟값에서 감소하면 최댓값으로 순환
			{
				p[rtc_ui.target_idx] = rtc_ui.max_val[rtc_ui.btn2_idx];
			}
			else p[rtc_ui.target_idx]--;
			
			rtc_ui.btn0_sub = 0;
		}
		else if (rtc_ui.btn1_add)
		{
			if (p[rtc_ui.target_idx] >= rtc_ui.max_val[rtc_ui.btn2_idx])		// 최댓값에서 증가하면 최솟값으로 순환
			{
				p[rtc_ui.target_idx] = rtc_ui.min_val[rtc_ui.btn2_idx];
			}
			else p[rtc_ui.target_idx]++;
			
			rtc_ui.btn1_add = 0;
		}
		
		lcd_write_data((p[rtc_ui.target_idx] / 10) + '0');
		lcd_write_data((p[rtc_ui.target_idx] % 10) + '0');
	}
	
	if (msec_count >= 1000)
	{
		msec_count = 0;		
	}
}

void calc_mode(char *buff)
{	
	if (get_button(BUTTON4, BUTTON4PIN))
	{
		clear_expression();
		clear_display_expression(buff);
		
		rtc_ui.btn4_cmd_display = 1;
		
		app_mode = MODE_RTC_CLOCK;	// CALC MODE -> RTC MODE
		return;
	}
	
	if (rtc_ui.btn4_cmd_display)
	{
		lcd_multi_calc();	// DISPLAY ON | CURSOR ON | BLINK ON
		rtc_ui.btn4_cmd_display = 0;
		//char buff[100]={0};
	}
	
	if (queue_empty() != TRUE)
	{
		calc_input(buff, read_queue());    // keypad를 통해 queue에 값이 들어오면 계산식 수행 
	}
	
	if (get_button(BUTTON0, BUTTON0PIN))
	{
		calc_input(buff, KEY_CLEAR);	   // reset 
	}
	else if (get_button(BUTTON1, BUTTON1PIN))
	{
		calc_input(buff, KEY_BACKSPACE);   // 마지막 값 제거 
	}
	else if (get_button(BUTTON2, BUTTON2PIN))
	{
		calc_input(buff, '(');             // 여는 괄호 '(' 삽입 
	}
	else if (get_button(BUTTON3, BUTTON3PIN))
	{
		calc_input(buff, ')');			   // 닫는 괄호 ')' 삽입 
	}
	
	if (msec_count >= 1000)
	{
		msec_count = 0;
		
		lcd_display_clc(buff);
	}
}