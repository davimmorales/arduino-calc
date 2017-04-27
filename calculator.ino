#include <Keypad.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(7,8,9,10,11,12); // Allocate pins to LCD

long num1,num2 ;
int total;
char operation,button;
char expression[15] = ' ';
int top;

top = 0;

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
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
lcd.begin(16,2); // initialize the lcd
}

char checknumber(char ch)
{
    if (ch >= '0' && ch <= '9') return 1; else return 0;
}

int evaluate(char expr[])
{
    int numbers[5]; int nsi = 0;
    char operators[5]; int osi = 0;
    char numbuf[16]; int nbi = 0;
    char ch; int  i = 0;

    while ((ch = expr[i]) != 0) {
        if (checknumber(ch))
        {
            numbuf[nbi++] = ch;
            if (!checknumber(expr[i + 1]))
            {
                numbuf[nbi] = 0; nbi = 0;
                sscanf(numbuf, "%lf", &numbers[nsi++]);
            }
        }
        else
        {
            while ((osi > 0) && (precedence(ch) <= precedence(operators[osi - 1])))
            {
                numbers[nsi - 2] = calculate(operators[osi - 1], numbers[nsi - 2], numbers[nsi - 1]);
                osi--; nsi--;
            }
            operators[osi++] = ch;
        }
        i++;
    }
    while (osi > 0) {
        numbers[nsi - 2] = calculate(operators[osi - 1], numbers[nsi - 2], numbers[nsi - 1]);
        osi--; nsi--;
    }
    return numbers[0];
}

int precedence(char ch)
{
    int precedence;
    switch (ch)
    {
    case '+':
    case '-':
        precedence = 0;
        break;
    case '*':
    case '/':
        precedence = 1;
        break;
    case '^':
        precedence = 2;
    }
    return precedence;
}

int calculate(char moperator, int num1, int num2)
{
    int result;
    switch (moperator)
    {
    case '+':
        result = num1 + num2;
        break;
    case '-':
        result = num1 - num2;
        break;
    case '*':
        result = num1 * num2;
        break;
    case '/':
        result = num1 / num2;
        break;
    default:
        printf("Invalid Operator\n");
        exit(-1);
    }
    return result;
}

void loop()
{
    button = customKeypad.getKey(); // Button read
    if(top<16){
    if(button=='0'){
      expression[top] = '0';
      top = top+1;
    }
    if(button=='1'){
      expression[top] = '1';
      top = top+1;
    }
    if(button=='2'){
      expression[top] = '2';
      top = top+1;
    }
    if(button=='3'){
      expression[top] = '3';
      top = top+1;
    }
    if(button=='4'){
      expression[top] = '4';
      top = top+1;
    }
    if(button=='5'){
      expression[top] = '5';
      top = top+1;
    }
    if(button=='6'){
      expression[top] = '6';
      top = top+1;
    }
    if(button=='7'){
      expression[top] = '7';
      top = top+1;
    }
    if(button=='8'){
      expression[top] = '8';
      top = top+1;
    }
    if(button=='9'){
      expression[top] = '9';
      top = top+1;
    }
    if(button=='('){
      expression[top] = '(';
      top = top+1;
    }
    if(button==')'){
      expression[top] = ')';
      top = top+1;
    }
    }
    if(button=='C'&&top>0){
      top = top-1;
    }
    if(button=='enter'){
      top=0;

    }



  lcd.setCursor(0,1);
  lcd.print('=');
  lcd.setCursor(1,1);
  lcd.print(total);
}
