#include <LiquidCrystal.h>
LiquidCrystal lcd(7,8,9,10,11,12); // Allocate pins to LCD
// Make sure you declare proper pins here

void setup() 
{

lcd.begin(16,2); // initialize the lcd
lcd.setCursor(0,0);
lcd.print("This is row 1");
lcd.setCursor(0,1);
lcd.print("This is row 2");

}
 
void loop() {
// Main loop
}
