/*
 * stack.c
 *
 * Created: 2026-06-30 오전 8:05:49
 *  Author: kccistc
 */ 
#include "stack.h"

/*
중위 표기법 -> 후위 표기법
1. 숫자는 그대로 출력한다.
2. 만약 스택이 비어있다면 연산자를 그냥 스택에 넣는다.
3. (스택의 top에 있는 연산자의 우선순위 < 현재 연산자의 우선순위) 이면 현재 연산자를 그냥 스택에 넣는다.
4. (스택의 top에 있는 연산자의 우선순위 >= 현재 연산자의 우선순위) 이면 2번 혹은 3번 상황이 될 때까지 
	pop 하여 출력하고 연산자를 스택에 넣는다.
5. 모든 수식을 다 사용했다면 스택이 빌 때까지 pop하여 출력한다.
6. 우선순위는 (더하기=빼기) < (곱하기=나누기)이다.
7. 여는 괄호는 스택에 그냥 추가한다.
8. 여는 괄호 다음에 오는 연산자는 그냥 스택에 추가한다.
9. 닫는 괄호는 여는 괄호가 나올 때까지 스택을 pop 하여 출력한다. 다 출력하고 난 뒤 괄호들은 버린다.

후위 표기법 사칙연산
1. 숫자는 스택에 그냥 추가한다.
2. 연산자가 나오면 숫자 2개를 pop 해서 계산한다.
3. 이때 먼저 pop 되는 숫자가 두 번째 값, 나중에 pop되는 숫자가 첫 번째 값으로 하여 계산해야 한다. 
   계산한 값은 다시 스택에 넣는다.
*/

void init(stack* pst)
{
	pst->top = -1;
	pst->max = sizeof(pst->data);
}

void push(stack* pst, char word)
{
	if (!is_full(pst))
	{
		pst->data[++pst->top] = word;
	}
}

char pop(stack* pst)
{
	char word;
	if (!is_empty(pst))
	{
		word = pst->data[pst->top];
		pst->data[pst->top--] = '\0';
		return word;
	}
	
	return '\0';
}

char peek(stack* pst)
{
	if (!is_empty(pst))
	{
		return pst->data[pst->top];
	}	
	
	return '\0';
}

void reverse(char* str, int len)
{
	char tmp;
	
	for (int i = 0; i < len / 2; i++)
	{
		tmp = str[i];
		str[i] = str[len - 1 - i];
		str[len - 1 - i] = tmp;
	}
}

uint8_t is_priority(stack* pst, char op)
{
	// 현재 연산자 우선순위 > 스택 TOP 연산자 우선순위 return 1;
	// 현재 연산자 우선순위 <= 스택 TOP 연산자 우선순위 return 0;
	switch (op)
	{
		case '+':
		case '-':
			return 0;
		case '*':
		case '/':
			if (peek(pst) == '+' || peek(pst) == '-') return 1; 
			else if (peek(pst) == '*' || peek(pst) == '/') return 0;
	}
}

char* convert_postfix(stack* pst, char* pbuff)
{	
	// pbuff: 중위 표기식 
	static char tmp[100];   // 후위 표기법 결과식 
	char* ptmp = tmp;		

	memset(tmp, 0, sizeof(tmp));
	
	while (*pbuff != '=')
	{
		if (*pbuff >= '0' && *pbuff <= '9')
		{
			*ptmp++ = *pbuff++;    // 피연산자는 그대로 결과식에 넣음    
			
			// 피연산자 다음에 연산자를 만나면 피연산자와 구분하기 위해 공백 처리
			if (!(*pbuff >= '0' && *pbuff <= '9')) *ptmp++ = ' ';	
		}
		else if (*pbuff == '+' || *pbuff == '-' || *pbuff == '*' || *pbuff == '/')  // 연산자를 만남 
		{	// 연산자를 만난 후, 스택이 비어있거나 스택 TOP에 여는 괄호'('가 오면 현재 연산자를 스택에 PUSH
			if (is_empty(pst) || peek(pst) == '(') push(pst, *pbuff++);
			else
			{
				// 현재 연산자가 스택 TOP에 있는 연산자 보다 우선순위가 낮거나 같으면
				// 스택이 비어있거나 현재 연산자가 스택 TOP의 연산자보다 우선순위가 높을때까지
				// 스택에서 POP하여 결과식에 넣고 현재 연산자를 스택에 PUSH
				// 연산자와 연산자 사이를 구분하기 위해 공백 처리 (결과식)	
				if (is_priority(pst, *pbuff)) push(pst, *pbuff++);
				else
				{
					// 현재 연산자의 우선순위가 TOP보다 낮거나 같으면 TOP 연산자를 하나 pop하여 출력
					// 이때 pbuff(중위 표기식)는 증가시키지 않으므로, 같은 연산자를 다음 반복에서 다시 검사
					*ptmp++ = pop(pst);
					*ptmp++ = ' ';
				} 
			}
		}
		else if (*pbuff == '(') push(pst, *pbuff++);
		// 현재 연산자가 여는 괄호 '('이면 스택에 PUSH
		else if (*pbuff == ')')
		{  // 현재 연산자가 닫는 괄호 ')'이면 여는 괄호 '('가 나올 때까지
		   // 스택에서 POP하여 결과식에 넣음 
			while (!is_empty(pst) && peek(pst) != '(')
			{
				*ptmp++ = pop(pst);  // 스택에서 POP한 값을 결과식에 넣음 
				*ptmp++ = ' ';		 // 연산자와 연산자 사이를 구분하기 위해 공백 처리 (결과식)
			}
			
			if (is_empty(pst))
			{	// 괄호가 누락된 경우 알림 출력 후 반목문 종료
				printf("missing '('\n");
			} 
			else
			{
				pop(pst);  // 여는 괄호 '(' 제거 
			}
			pbuff++; // 닫는 괄호')' 처리가 끝났으므로 다른 문자로 이동 
		}
		else if (*pbuff == ' ') pbuff++; // 중위 표기식에 공백이 있을 경우 다음 index를 가리킴 
	}
	
	while (!is_empty(pst))
	{	// 모든 수식을 처리한 뒤, 스택에 남아 있는 연산자를 모두 POP하여 결과식에 넣음
		*ptmp++ = pop(pst);
		// 연산자와 연산자 사이를 구분하기 위해 공백 처리 (결과식)
		*ptmp++ = ' ';
	}
	
	// ptmp는 현재 후위 표기식의 끝 다음 위치를 가리킴
	// 결과식 끝에 불필요한 공백이 있으면 제거하기 위해
	// 마지막 문자(*(ptmp - 1))가 공백인지 확인
	// ptmp > tmp는 결과식이 비어 있을 때 tmp[-1]에 접근하지 않기 위한 조건임
	// ptmp > tmp: ptmp가 tmp 배열의 시작 위치보다 뒤에 있는지 확인  
	// *ptmp = '\0': 마지막 공백에 널 문자 '\0'를 넣기 위함 
	if (ptmp > tmp && *(ptmp - 1) == ' ')
	{
		ptmp--;
	}
	
	// 문자열의 끝을 표시한다.
	// 만약 마지막 문자가 공백이었담녀 해당 공백 위치에 '\0'이 들어가면서 공백이 제거됨	
	*ptmp = '\0';
	
	return tmp;
}

int pop_operand(stack* pst)
{
	char tmp[100] = { 0, };
	char *ptmp = tmp;
	
	// 피연산자 뒤쪽 공백 제거
	if (peek(pst) == ' ')
	{
		pop(pst);
	}
	
	// 다음 공백을 만나기 전까지 숫자 문자만 POP
	while(!is_empty(pst) && peek(pst) != ' ')
	{
		*ptmp++ = pop(pst);
	}
	
	// 피연산자 앞쪽 공백 제거
	if (peek(pst) == ' ')
	{
		pop(pst);
	}
	
	// 스택은 LIFO 구조라 숫자가 뒤에서부터 꺼내지므로,
	// 원래 숫자 순서로 만들기 위해 문자열을 반전
	reverse(tmp, strlen(tmp));		
	
	return atoi(tmp);
}
int cal_postfix(stack* pst, char* pbuff)
{
	// pbuff: 후위 표기식
	char token[20] = {0, };
	char* ptoken;
	int num1, num2;
	int result = 0;

	// 첫 번째 피연산자를 꺼낼 때도 공백 구분자를 만날 수 있도록
	// 스택 맨 아래에 초기 구분자 역할의 공백을 넣음 
	push(pst, ' ');
	
	while (*pbuff != '\0')  
	{
		if (*pbuff >= '0' && *pbuff <= '9' || *pbuff == ' ')
		{	// 후위 표기식에서 꺼낸 값이 숫자이거나 공백인 경우,
			// 그대로 스택에 PUSH
			push(pst, *pbuff++);
		}
		else
		{	
			// 연산자를 만나면 피연산자 2개를 꺼내 계산 
			// 먼저 꺼낸 값이 두 번째 피연산자
			num2 = pop_operand(pst);	
			num1 = pop_operand(pst);
			
			result = op(*pbuff, num1, num2);  
			
			sprintf(token, "%d", result);	  // 계산 결과를 문자열 형태로 만들어서 CHAR 배열에 저장
			push(pst, ' ');					  // 계산 결과 앞쪽 구분자 
			
			ptoken = token;					 
			while (*ptoken != '\0')			  // 널 문자를 만나기 전까지 수행 
			{
				push(pst, *ptoken++);		  // 계산 결과를 다시 스택에 PUSH
			}
			pbuff++;
			
			// 연산자 뒤에 있는 공백은 입력 구분자이므로 건너뜀 
			// 계산 결과 뒤의 공백은 코드에서 직접 넣기 때문에, 
			// 입력 공백까지 다시 push하면 공백이 중복될 수 있음
			while (*pbuff == ' ')			 
			{	 // 후위 표기식에서 공백을 만나면 다음 인덱스로 넘어감
				pbuff++;
			}
			
			if (*pbuff != '\0')				  
			{	// 아직 처리할 토큰이 남아 있으면 결과값 뒤에 구분자 공백을 추가 
				push(pst, ' ');
			}
		}
	}
	
	return pop_operand(pst);
}

int op(char c, int num1, int num2)
{
	switch (c)
	{
	case '+':
		return num1 + num2;
	case '-':
		return num1 - num2;
	case '*':
		return num1 * num2;
	case '/':
		return num1 / num2;
	}
}

int is_empty(stack* pst)
{
	return pst->top == -1;
}

int is_full(stack* pst)
{
	if (pst->top + 1 >= pst->max)
	{
		printf("stack is full !!\n");
		return 1;
	}
	else
	{
		return 0;
	}
}