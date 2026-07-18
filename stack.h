/*
 * stack.h
 *
 * Created: 2026-06-30 오전 8:07:47
 *  Author: kccistc
 */ 

#define F_CPU 16000000UL  // 16MHz
#include <avr/io.h>  // PORTA PORTB PORTD... IO관련 reg가 들어 있다.
#include <util/delay.h>  // _delay_ms _delay_us 등
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	int top;
	int max;
	char data[100];
} stack;

void init(stack* pst);
void push(stack* pst, char word);
char pop(stack* pst);
char peek(stack* pst);
void reverse(char* str, int len);
uint8_t is_priority(stack* pst, char op);
char* convert_postfix(stack* pst, char* pbuff);
int pop_operand(stack* pst);
int cal_postfix(stack* pst, char* pbuff);
int op(char c, int num1, int num2);
int is_empty(stack* pst);
int is_full(stack* pst);