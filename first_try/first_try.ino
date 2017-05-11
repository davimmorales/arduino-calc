
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 15

LiquidCrystal lcd(7,8,9,10,11,12); // Allocate pins to LCD
const int PwPin = A0;
int PwValue = 0;
const int light_output = 13;
int eeaux = 0;

long num1,num2 ;
double total;
char operation,button;
int stack[STACK_SIZE];
int top;



const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'C','0','=','/'}
};
byte rowPins[ROWS] = {A2,A3,A4,A5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2,3,4,5}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  analogReference(INTERNAL1V1);
  pinMode(light_output, OUTPUT);
  digitalWrite(13, LOW);
  lcd.begin(16,2); // initialize the lcd
}

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
    button = customKeypad.getKey(); // Button read
    number_str[counter] = button;
    counter++;
  }

  if(counter>5){
    exit_number_too_big();
  }

  number = atoi(number_str);
  push(number);

  if ( button == '+'){
    push(pop() + pop());
    lcd.setCursor(0,0); // Select first row on lcd
    lcd.print("+"); // Print current number1
  }
  else if ( button == '-'){
    subtract();
    lcd.setCursor(0,0); // Select first row on lcd
    lcd.print("-"); // Print current number1
  }
  else if ( button == '*'){
    push(pop() * pop());
    lcd.setCursor(0,0); // Select first row on lcd
    lcd.print("*"); // Print current number1
  }
  else if ( button == '/'){
    divide();
    lcd.setCursor(0,0); // Select first row on lcd
    lcd.print("/"); // Print current number1
  }else{
    lcd.setCursor(0,0); // Select first row on lcd
    lcd.print(","); // Print current number1
  }

}

void loop()
{
  PwValue = analogRead(PwPin);
        analogWrite(light_output, 1/(PwValue/4));
  int EEPROM_value = 0;

  int value;
      while(1) {
      PwValue = analogRead(PwPin);
      analogWrite(light_output, 1/(PwValue/4));
      button = customKeypad.getKey(); // Button read
      if (is_number(button)){
        insert_number(button);
        lcd.setCursor(0,0); // Select first row on lcd
        lcd.print(num1); // Print current number1
      }
      else if ( button == '+'){
        push(pop() + pop());
        lcd.setCursor(0,0); // Select first row on lcd
        lcd.print("+"); // Print current number1
      }
      else if ( button == '-'){
        subtract();
        lcd.setCursor(0,0); // Select first row on lcd
        lcd.print("-"); // Print current number1
      }
      else if ( button == '*'){
        push(pop() * pop());
        lcd.setCursor(0,0); // Select first row on lcd
        lcd.print("*"); // Print current number1
      }
      else if ( button == '/'){
        divide();
        lcd.setCursor(0,0); // Select first row on lcd
        lcd.print("/"); // Print current number1
      }
      else
        break;
    }

      if (button == '=') {
        EEPROM.write(eeaux,PwValue/4);
        EEPROM_value = EEPROM.read(eeaux);
            eeaux++;
        Serial.print("EEPROM luminosity value = ");
        Serial.print(EEPROM_value);

        value = pop();
        if (is_empty()) {
          printf("%d\n",  value);
          lcd.setCursor(1,1);
          lcd.print('=');
          lcd.setCursor(1,1);
          lcd.print(value);
        } else {
          warn_incomplete_expression();
          empty();
        }
      }
    }
    // // Loops are convenient for reading key press from keypad
    // while(1) // First loop. Here we read keypad and compose our first number. It does so untill we press operation button and break's out of loop or 'C' and it starts from beginning of this loop
    // {
    //
    //
    //
    //     if (button=='C') // If user wants to resset while he is writing first number
    //     {
    //       num1=0;
    //       num2=0;
    //       total=0;
    //       operation=0;
    //       lcd.clear();
    //     }
    //
    //     if (button >='0' && button <='9') // If user pressed numeric value, 1 character at a time.
    //     {
    //       num1 = num1*10 + (button -'0'); // Our numeric values are 0-9 witch means they are in first decade, when we multiply with 10 we basicaly add zero after number,
    //       // than we add a new number entered to that zero spot. As for (button -'0') this is simple ASCII table "trick" 0...9 in ASCII table are 48 ... 57,
    //       // so by subtracting '0' from any of them we get their value in decade system ex. char '5' = 53 in decade numeric system minus 48 for zero char gives us value of actual 5,
    //       // if our previous number was ex. 25 we get 250 by multiplying it with 10 and then we add 5 and we get 255 witch gets printed on LCD.
    //       lcd.setCursor(0,0); // Select first row on lcd
    //       lcd.print(num1); // Print current number1
    //     }
    //     if (num1 !=0 && (button=='-' || button=='+' || button=='*' || button=='/')) // If user is done inputing numbers
    //     {
    //       operation = button; // operation remembers what mathematical operation user wants on numbers
    //       lcd.setCursor(0,1); // set cursor to row 2
    //       lcd.print(operation); // print our operator
    //       break;
    //     }
    //
    // }
    //
    // while(1) // Second while loop, it loops untill user has pressed '=' or 'C'. so it either prints total or ressets program
    // {
    //     if (button =='C'){break;} // This covers case where user pressed operator and still wants to reset
    //     button = customKeypad.getKey();
    //     if (button=='C') // Making sure user wants to reset at anytime
    //     {
    //       num1=0;
    //       num2=0;
    //       total=0;
    //       operation=0;
    //       lcd.clear();
    //       break;
    //     }
    //     if (button >='0' && button <='9') // Getting chars from keypad for number 2
    //     {
    //       num2 = num2*10 + (button -'0');
    //       lcd.setCursor(1,1);
    //       lcd.print(num2);
    //     }
    //     if (button == '=' && num2 !=0)// If button pressed was '=' its the end of the road. Calls domath() subroutine does calculation and print our results
    //     {
    //       domath();
    //
    //       break;
    //     }
    // }
//     while(1)
//     {
//       // After all is done this loop waits for 'C' key to be pressed so it can reset program and start over.
//       if (button =='C'){break;} // This line is side effect of previous loop since if user pressed 'C' it breaks out of previous loop and continues here.So we need to break this one aswell or user would need to press 'C' 2 times
//       button = customKeypad.getKey();
//       if (button =='C')
//         {
//            lcd.clear();
//            lcd.setCursor(0,0);
//            num1=0;
//            num2=0;
//            total=0;
//            operation=0;
//            break;
//         }
//     }
//
// }
// void domath() // Simple switch case to pick what operation to do, based on button pressed by user.
// {
//   switch(operation)
//     {
//       case '+': // Addition
//           total = num1+num2;
//           break;
//
//       case '-': // Subtraction
//           total = num1-num2;
//           break;
//
//       case '/': // Division. Could add error for division by zero, or change line in second loop where it waits for '=' char to if (button == '=' && num2 != 0) this will halt program untill num2 is not zero and then continue
//           total = (float)num1/(float)num2;
//           break;
//
//       case '*': // Multiplication
//           total = num1*num2;
//           break;
//
//     }
//     // Based on case selected print our total and
//     lcd.setCursor(0,1);
//     lcd.print('=');
//     lcd.setCursor(1,1);
//     lcd.print(total);
//
// }

