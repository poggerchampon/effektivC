#include <stdio.h>
#include <ctype.h>
#define N		(10)

int stack[N];
int index = -1;
unsigned int line = 1;
void skip_line(){
	int c;
	while((c = getchar()) != '\n'){

	}
	index = -1;
	line +=1;
}
void error(int c){
	printf("line %d: error at %d\n", line, c);
	skip_line();
}
void error_operator(int c){
	printf("line %d: error at %c\n", line, c);
	skip_line();
}
void error_newline(){
	printf("line %d: error at \\n\n", line);
}


int is_whitespace(int c) {
	return c == ' ' || c == '\n' || c == '\t';
}

int is_op(int c){
	return c == '*' || c == '/' || c == '-' || c == '+';
}

int is_legal_char(int c){
	return isdigit(c) || is_op(c) || is_whitespace(c);
}
int is_empty(){
	return index == -1;
}
int is_full(){
	return index == N-1;
}
int push(int i){
	if (is_full()){
		error(i);
		return -1;
	}
	index += 1;
	stack[index] = i;
	return i;
}
int pop(){
	if (is_empty()){
		return -1;
	}
	int ret_val = stack[index];
	index -= 1;
	return ret_val;
}
int handle_digit(int c){
	int num = 0;
	int k = -1;
	while (isdigit(c)){
		num = num * 10 + (c-'0');
		c = getchar();
		if (!isdigit(c)){
			k = c;
		}
	}
	push(num);
	return k;
}

void resolve_operator(int c){
	int rhs = pop();
	int lhs = pop();
	if (rhs == -1 || lhs == -1){
		error_operator(c);
	}
	switch (c)
	{
	case '+':
		push(lhs + rhs);
		break;
	case '*':
		push(lhs * rhs);
		break;
	case '/':
		if (rhs == 0){
			error_operator(c);
			break;
		}
		push(lhs / rhs);
		break;
	case '-':
		push(lhs - rhs);
		break;
	default:
		break;
	}
}

void resolve_whitespace(int c){
	if (c != '\n'){
		return;
	}
	if (index == 0){
		printf("line %d: %d\n", line, pop());
	} else {
		error_newline();
		index = -1;
	}
	line+=1;
}

int main(void)
{
	int c = 0;
	while ((c = getchar()) != EOF){
		start:
		if (is_whitespace(c)){
			resolve_whitespace(c);
			continue;
		}
		if (!is_legal_char(c)){
			error_operator(c);
			continue;
		}
		if (isdigit(c)){
			c = handle_digit(c);
			goto start;
		}
		if (is_op(c)){
			resolve_operator(c);
			continue;
		}
		
	}
	return 0;
}

