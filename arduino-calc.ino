// Embedded Systems
// Student: Davi Morales
// RA: 69550

#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define STACK_SIZE 15 //maximum size for the stack that fits the LCD display


int eeprom_position = 0;//EEPROM initialized

LiquidCrystal lcd(7,8,9,10,11,12); // pins allocated to LCD

int num1;
char operation,button;
String expression;

const byte ROWS = 4;
const byte COLS = 4;

int photocellPin = 0;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider
int LCDpin = 13;          // the pin that will determine the brightness of the display
int LCDbrightness;        // the variable that will keep the current value for brightness

char keys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {',','0','=','/'}
};

// the following rows and columns are connected to the keypad, which is defined
// so keys are mapped to their respective values
byte rowPins[ROWS] = {A2,A3,A4,A5};
byte colPins[COLS] = {2,5,4,3};
Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
void setup() {
  Serial.begin(9600); // serial port initialized
  pinMode(13, OUTPUT); // pin defined as output
  digitalWrite(13, LOW); // pin initialized
  lcd.begin(16,2); // lcd initialized
}

/* globals */
int stack[STACK_SIZE];//int stack declared
int top;// the top of the stack

/* prototypes */
bool is_empty(void);// returns 0 if stack top is at 0 (empty stack)
bool is_full(void);// returns 0 if stack top has reached its limit size
void push(int operand);// pushes a number into the stack
char pop(void);//pops a value from the stack
void subtract(void);//pops two values, subtracts them and then pushes the result
void divide(void);//pops two values, divides them and then pushes the result
void stack_overflow(void);// exits when the user tries to push into a full stack
void stack_underflow(void);// exits when the user tries to pop from an empty stack

bool is_empty(void){
  return top == 0;
}

bool is_full(void){
  return top == STACK_SIZE;
}

void push(int operand){
  if (is_full())
    stack_overflow();
  stack[top++] = operand;
}

char pop(void){
  if (is_empty())
    stack_underflow();
  return stack[--top];
}

void stack_overflow(void){
  Serial.print("Invalid expression.\n");
  exit(EXIT_FAILURE);
}

void stack_underflow(void){
  Serial.print("Invalid expression.\n");
  exit(EXIT_FAILURE);
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

//tells whether an entered string represents a value from 1 to 9 or not
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

    // in this loop, the expression is retrieved and stored into the array
    // 'expression'
    while(1)
    {
  // reads ambient luminance
  photocellReading = analogRead(photocellPin);

  // writes current ambient luminance into a given sequential position on the
  // EEPROM
  EEPROM.write(eeprom_position, photocellReading);

  // the reading is inverted from 0-1023 back to 1023-0
  photocellReading = 1023 - photocellReading;

  // here, 0-1023 is mapped to 0-80 in order to attend to the brightness needs
  // of the display according to ambient luminance
  LCDbrightness = map(photocellReading, 0, 1023, 0, 80);
  // the value defined is then written to the display through the PWM
  analogWrite(LCDpin, LCDbrightness);
  // the current brightness level is then retrieved from the EEPROM and displayed
  // at the serial monitor
  Serial.print("Brightness level: ");
  Serial.print(EEPROM.read(eeprom_position));
  Serial.print("\n");
  eeprom_position++;
  delay(100);


        button = customKeypad.getKey(); // button is read

        // inputs numeric values
        if (button >='0' && button <='9')
        {
          // converts entry to string and adds it to a given sequential position in
          // the expression
          num1 = button - '0';
          expression = String(expression+num1);
          lcd.setCursor(0,0); // first row selection on LCD
          position++;
          lcd.print(expression); // prints current expression
        }

        // inputs the operations into the expression
        if (num1 !=0 && (button=='-' || button=='+' || button=='*' || button=='/'))
        {
          operation = button;
          expression = String(expression+operation);
          position++;
          lcd.setCursor(0,0);
          lcd.print(expression);
        }
        // if '=' is pressed, then the operation is over and the result can be displayed
        if(button=='='){break;}
    }
    String ch;
    String ch_simple;
    const char *ch_char;
    int number_int;
    // goes through the expression in order to perform the operations on the
    // numbers
    for(i=0;i<16;i++){
      ch = String(expression.charAt(i));

      // compares each char from the expression.
      // if it is a number, it's pushed into the stack
      if (is_number(ch)){
        ch_char = static_cast<const char*>(ch.c_str());
        number_int = atoi(ch_char);
        push(number_int);
      }
      // else if it corresponds to an operation, it can add the last 2 pop values,
      // multiply them or perform a subtraction or division as mentioned earlier
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

      // displays result as the last needed pop from the stack,
      // which has gone through all the necessary calculations
      int result;
      result = pop();
      lcd.setCursor(0,1); // set cursor to row 2
      lcd.print(result); // print our operator




    while(1)
    {
      // after the result is shown, whenever the ',' key is pressed, the system restarts
      if (button ==','){break;} // breaks continuous loop
      button = customKeypad.getKey();
      if (button ==',')
        {
           lcd.clear();
           lcd.setCursor(0,0);
           num1=0;
           operation=0;
           expression = String();
           break;
        }

        // ambient luminance analysis and posting as seen before in the file
        photocellReading = analogRead(photocellPin);
        EEPROM.write(eeprom_position, photocellReading);
        photocellReading = 1023 - photocellReading;
        LCDbrightness = map(photocellReading, 0, 1023, 0, 80);
        analogWrite(LCDpin, LCDbrightness);
        Serial.print("Brightness level: ");
        Serial.print(EEPROM.read(eeprom_position));
        Serial.print("\n");
        eeprom_position++;
        delay(100);
    }

}
