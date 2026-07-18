/*
 * cal.c
 *
 * Created: 2026-06-29 오후 4:20:51
 *  Author: kccistc
 */ 

#include "cal.h"
#include "lcd.h"
#include "stack.h"
#include "button.h"

static char expr_buff[100];   // 계산에 사용할 중위 표기식 저장 버퍼
static int disp_idx = 0;
static int expr_idx = 0;      // 계산용 버퍼 인덱스


void calc_input(char *pbuff, uint8_t k_data)
{	
	// pbuff: lcd에 출력할 display용 버퍼
	stack st;
	init(&st);
	
	uint8_t flag=0;
	char *ptmp;
	int result;
	int res_idx;
	
	if (k_data == KEY_CLEAR)
	{
		lcd_clear();
		clear_expression();  // 계산 도중에 계산용 버퍼 초기화를 위해 필요함
		clear_display_expression(pbuff);
	}
	else if (k_data == KEY_BACKSPACE)
	{
		if (disp_idx > 0)
		{
			uint8_t last = pbuff[disp_idx - 1];
			 
			disp_idx--;
			pbuff[disp_idx] = '\0';
			
			if (last >= '0' && last <= '9')
			{
				if (expr_idx > 0)
				{
					expr_idx--;
					expr_buff[expr_idx] = '\0';		
				}
			}
			else 
			{
				// 연산자/괄호는 expr_buff에 ' + ' 처럼 3글자로 들어감 
				for (int i = 0; i < 3 && expr_idx > 0; i++)
				{
					expr_idx--;
					expr_buff[expr_idx] = '\0';
				}
			}
			
			lcd_remove_data(disp_idx);
			if (disp_idx >= 16) shift_display_right();		
		}
	}
	else 
	{
		// '='이 아닌 일반 입력은 최대 입력 길이를 넘지 못하게 제한
		if (k_data != '=' && disp_idx >= CAL_MAX_INPUT_LEN) return;		// 입력 무시 
		
		// display용 버퍼 자체 보호
		if (disp_idx + 1 >= CAL_DISPLAY_BUF_SIZE) return;
		
		flag = build_expression(expr_buff, k_data);  // keypad로부터 입력받은 값을 중위 표기식에 삽입
		
		if (flag == CAL_REJECT) return; // expr_buff에 더 이상 넣을 공간이 없으면 display에도 넣지 않음 
				
		if (disp_idx >= 16) shift_display_left();
		
		pbuff[disp_idx++] = k_data;		// 계산에 사용되는 중위 표기식과는 별도의 버퍼(lcd display용)에
		pbuff[disp_idx] = '\0';			// keypad 입력값 삽입, 입력값 다음에는 널문자 삽입
	}
	
	if (flag == CAL_EXECUTE) 
	{
		ptmp = convert_postfix(&st, expr_buff);  // 중위 표기식 -> 후위 표기식 변환
		
		init(&st);	// 스택 초기화 
		result = cal_postfix(&st, ptmp);  // 후위 연산자 수행 
		
		int remain = LCD_LINE_MEM_SIZE - disp_idx;
		
		if (remain > 0)
		{
			// snprintf(저장할_버퍼, 버퍼_크기, "형식 문자열", 값);
			snprintf(&pbuff[disp_idx], remain + 1, "%d", result);  // 숫자를 문자형으로 변환해서 diplay용 버퍼에 삽입	
		}	
		
		res_idx = strlen(pbuff);	// display용 버퍼 index 업데이트 
		
		while (1)
		{
			if (disp_idx >= 16) shift_display_left();
			
			disp_idx++;
			
			if (disp_idx > res_idx)
			{
				disp_idx--;
				break;
			}
		}
		
		clear_expression();	
	}
}

uint8_t build_expression(char *pbuff, uint8_t val)
{
	uint8_t need = 0;
	
	if (val == '=') need = 2;   // ' ' + '=': '=' 기호를 식에 넣기 위해 앞 구분자 공백' '이랑 '='을 넣을 공간 2자리가 필요함 
	else if (val >= '0' && val <= '9') need = 1;  // 숫자는 구분자 공백 없이 바로 값을 넣으므로 1자리만 필요함 
	else need = 3;	// val: + - * / ( )		// 그외 연산자 및 괄호는 앞/뒤 구분자 공백' '이랑 연산자/괄호를 넣을 공간 3자리가 필요함 
	
	// need만큼 넣은 뒤 마지막에 '\0'까지 들어갈 공간이 있어야 함
	if (expr_idx + need >= CAL_EXPR_BUF_SIZE) return CAL_REJECT;
	
	if (val == '=')
	{	// '=' 값을 받는다면, 즉 계산식이 온전히 만들어지면
		// '=' 기호 앞에는 공백 구분자를 삽입
		// '=' 기호 뒤에는 널 문자를 삽입
		pbuff[expr_idx++] = ' ';
		pbuff[expr_idx++] = val;
		pbuff[expr_idx] = '\0';
		return CAL_EXECUTE;
	}		
	else if (val >= '0' && val <= '9')
	{
		// 피연산자를 받는다면 중위 표기식에 피연산자 삽입 
		pbuff[expr_idx++] = val;
		pbuff[expr_idx] = '\0';
		return CAL_CONTINUE;
	}
	else	// val: + - * / ( )
	{	
		// '='을 제외한 연산자 또는 괄호를 앞뒤 공백 구분자를 넣어서 삽입 
		pbuff[expr_idx++] = ' ';
		pbuff[expr_idx++] = val;
		pbuff[expr_idx++] = ' ';
		pbuff[expr_idx] = '\0';
		return CAL_CONTINUE;
	}
	
	return 0;
}

void clear_expression(void)
{
	memset(expr_buff, 0, sizeof(expr_buff));  // 계산용 중위 표기식 버퍼 초기화
	expr_idx = 0;					// 계산용 버퍼 인덱스 초기화 
}

void clear_display_expression(char *pbuff)
{
	memset(pbuff, 0, 100);			// display용 버퍼 초기화
	disp_idx = 0;					// display용 버퍼 인덱스 초기화 
}