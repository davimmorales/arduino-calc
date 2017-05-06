/* Reverse Polish Notation calculator */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 100

/* globals */
int stack[STACK_SIZE];
int top;


/* prototypes */
void empty(void);
bool is_empty(void);
bool is_full(void);
void push(int operand);
char pop(void);
void subtract(void);
void divide(void);
void exit_stack_overflow(void);
void exit_stack_underflow(void);
void exit_number_too_big(void);
void warn_incomplete_expression(void);


void empty(void){
  top = 0;
}

bool is_empty(void){
  return top == 0;
}

bool is_full(void){
  return top == STACK_SIZE;
}

void push(int operand){
  if (is_full())
    exit_stack_overflow();
  stack[top++] = operand;
}

char pop(void){
  if (is_empty())
    exit_stack_underflow();
  return stack[--top];
}

void exit_stack_overflow(void){
  fprintf(stderr, "Expression is too complex.\n");
  exit(EXIT_FAILURE);
}

void exit_stack_underflow(void){
  fprintf(stderr, "Not enough operands in expression\n");
  exit(EXIT_FAILURE);
}

void exit_number_too_big(void){
  fprintf(stderr, "Number is too big\n");
  exit(EXIT_FAILURE);
}

void warn_incomplete_expression(void){
  fprintf(stderr, "Incomplete expression\n");
}

void subtract(void){
  int subtrahend = pop();
  int minuend = pop();

  push(minuend - subtrahend);
}

void divide(void){
  int divisor = pop();
  int dividend = pop();

  push(dividend / divisor);
}

bool is_number(char c){
  if (c >= '0' && c <= '9')
    return 1;
  else
    return 0;
}

void insert_number(char ch){
  char digit = ch;
  char number_str[5];
  int number;
  int counter;

  counter = 1;
  number_str[0] = digit;

  while(is_number(digit)){
    scanf(" %1c", &digit);
    number_str[counter] = digit;
    counter++;
  }

  if(counter>5){
    exit_number_too_big();
  }

  number = atoi(number_str);
  push(number);

  if ( digit == '+')
    push(pop() + pop());
  else if ( digit == '-')
    subtract();
  else if ( digit == '*')
    push(pop() * pop());
  else if ( digit == '/')
    divide();
}

int main(void)
{
  char ch;
  int value;

    while(1) {
      scanf(" %1c", &ch);
      if (is_number(ch)){
        insert_number(ch);
      }
      else if ( ch == '+')
        push(pop() + pop());
      else if ( ch == '-')
        subtract();
      else if ( ch == '*')
        push(pop() * pop());
      else if ( ch == '/')
        divide();
      else
        break;
    }

    if (ch == '=') {
      value = pop();
      if (is_empty()) {
        printf("%d\n",  value);
      } else {
        warn_incomplete_expression();
        empty();
      }
    }

  return 0;
}
