#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define STACK_SIZE 15


int eeprom_position = 0;

LiquidCrystal lcd(7,8,9,10,11,12); // Allocate pins to LCD

int num1,num2 ;
double total;
char operation,button;
String expression;

const byte ROWS = 4;
const byte COLS = 4;

int photocellPin = 0;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider
int LCDpin = 13;          // connect Red LED to pin 11 (PWM pin)
int LCDbrightness;        //

char keys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {',','0','=','/'}
};

byte rowPins[ROWS] = {A2,A3,A4,A5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2,5,4,3}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
lcd.begin(16,2); // initialize the lcd
}

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

bool is_number(String c){
  if (c >= String('0') && c <= String('9'))
    return 1;
  else
    return 0;
}


void loop()
{
  int position = 0;
  int i;


    // Loops are convenient for reading key press from keypad
    while(1) // First loop. Here we read keypad and compose our first number. It does so untill we press operation button and break's out of loop or 'C' and it starts from beginning of this loop
    {
  photocellReading = analogRead(photocellPin);

  //Serial.print("Analog reading = ");
  //Serial.println(photocellReading);     // the raw analog reading
  EEPROM.write(eeprom_position, photocellReading);

  // LED gets brighter the darker it is at the sensor
  // that means we have to -invert- the reading from 0-1023 back to 1023-0
  photocellReading = 1023 - photocellReading;


  //now we have to map 0-1023 to 0-255 since thats the range analogWrite uses
  LCDbrightness = map(photocellReading, 0, 1023, 0, 80);
  analogWrite(LCDpin, LCDbrightness);
  Serial.print("Brightness level: ");
  Serial.print(EEPROM.read(eeprom_position));
  Serial.print("\n");
  eeprom_position++;
  delay(100);


        button = customKeypad.getKey(); // Button read

        if (button >='0' && button <='9') // If user pressed numeric value, 1 character at a time.
        {
          num1 = button - '0';
          expression = String(expression+num1);

          num1 = num1*10 + (button -'0'); // Our numeric values are 0-9 witch means they are in first decade, when we multiply with 10 we basicaly add zero after number,
          // than we add a new number entered to that zero spot. As for (button -'0') this is simple ASCII table "trick" 0...9 in ASCII table are 48 ... 57,
          // so by subtracting '0' from any of them we get their value in decade system ex. char '5' = 53 in decade numeric system minus 48 for zero char gives us value of actual 5,
          // if our previous number was ex. 25 we get 250 by multiplying it with 10 and then we add 5 and we get 255 witch gets printed on LCD.
          lcd.setCursor(0,0); // Select first row on lcd
          position++;
          lcd.print(expression); // Print current number1
        }
        if (num1 !=0 && (button=='-' || button=='+' || button=='*' || button=='/')) // If user is done inputing numbers
        {
          operation = button; // operation remembers what mathematical operation user wants on numbers
          expression = String(expression+operation);
          position++;
          lcd.setCursor(0,0); // set cursor to row 2
          lcd.print(expression); // print our operator
        }
        if(button=='='){break;}
    }
    String ch;
    String ch_simple;
    const char *ch_char;
    int number_int;
    for(i=0;i<16;i++){
      ch = String(expression.charAt(i));

      if (is_number(ch)){
        // while(1){
        //   ch_simple = String(expression.charAt(i));
        //   ch = ch+String(expression.charAt(i));
        //   i++;
        //   if(!is_number(ch_simple));
        //     break;
        // }
        ch_char = static_cast<const char*>(ch.c_str());
        number_int = atoi(ch_char);
        push(number_int);

      }
      else if ( ch == String('+'))
        push(pop() + pop());
      else if ( ch == String('-'))
        subtract();
      else if ( ch == String('*'))
        push(pop() * pop());
      else if ( ch == String('/'))
        divide();
      else if(ch==String(','))
        printf(" ");
      else
        break;
    }

      int result;
      result = pop();
      lcd.setCursor(0,1); // set cursor to row 2
      lcd.print(result); // print our operator




    while(1)
    {
      // After all is done this loop waits for 'C' key to be pressed so it can reset program and start over.
      if (button ==','){break;} // This line is side effect of previous loop since if user pressed 'C' it breaks out of previous loop and continues here.So we need to break this one aswell or user would need to press 'C' 2 times
      button = customKeypad.getKey();
      if (button ==',')
        {
           lcd.clear();
           lcd.setCursor(0,0);
           num1=0;
           num2=0;
           total=0;
           operation=0;
           expression = String();
           break;
        }

        photocellReading = analogRead(photocellPin);

        //Serial.print("Analog reading = ");
        //Serial.println(photocellReading);     // the raw analog reading
        EEPROM.write(eeprom_position, photocellReading);

        // LED gets brighter the darker it is at the sensor
        // that means we have to -invert- the reading from 0-1023 back to 1023-0
        photocellReading = 1023 - photocellReading;


        //now we have to map 0-1023 to 0-255 since thats the range analogWrite uses
        LCDbrightness = map(photocellReading, 0, 1023, 0, 80);
        analogWrite(LCDpin, LCDbrightness);
        Serial.print("Brightness level: ");
        Serial.print(EEPROM.read(eeprom_position));
        Serial.print("\n");
        eeprom_position++;
        delay(100);
    }

}
