/*
 * cal.h
 *
 * Created: 2026-06-29 오후 4:21:02
 *  Author: kccistc
 */ 

#define F_CPU 16000000UL  // 16MHz
#include <avr/io.h>  // PORTA PORTB PORTD... IO관련 reg가 들어 있다.
#include <util/delay.h>  // _delay_ms _delay_us 등
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAL_DISPLAY_BUF_SIZE 100
#define CAL_EXPR_BUF_SIZE	 100

#define LCD_LINE_MEM_SIZE	 40
#define CAL_RESULT_RESERVE	 7

#define CAL_MAX_INPUT_LEN	 (LCD_LINE_MEM_SIZE - CAL_RESULT_RESERVE - 1)

#define CAL_CONTINUE		 0
#define CAL_EXECUTE			 1
#define CAL_REJECT			 2

void clear_expression(void);
void clear_display_expression(char *pbuff);
void calc_input(char *pbuff, uint8_t k_data);
uint8_t build_expression(char *pbuff, uint8_t val);

