#include <LiquidCrystal.h>
#include <EEPROM.h>

int eeprom_position = 0;

/* Photocell simple testing sketch.

Connect one end of the photocell to 5V, the other end to Analog 0.
Then connect one end of a 10K resistor from Analog 0 to ground
Connect LED from pin 11 through a resistor to ground
For more information see http://learn.adafruit.com/photocells */

LiquidCrystal lcd(7,8,9,10,11,12); // Allocate pins to LCD
int photocellPin = 0;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider
int LCDpin = 2;          // connect Red LED to pin 11 (PWM pin)
int LCDbrightness;        // 
void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);
  lcd.begin(16, 2);
}

void loop(void) {
  photocellReading = analogRead(photocellPin);

  //Serial.print("Analog reading = ");
  //Serial.println(photocellReading);     // the raw analog reading
  EEPROM.write(eeprom_position, photocellReading);

  // LED gets brighter the darker it is at the sensor
  // that means we have to -invert- the reading from 0-1023 back to 1023-0
  photocellReading = 1023 - photocellReading;
  //now we have to map 0-1023 to 0-255 since thats the range analogWrite uses
  LCDbrightness = map(photocellReading, 0, 1023, 0, 255);
  analogWrite(LCDpin, LCDbrightness);
  Serial.print("Brightness level: ");
  Serial.print(EEPROM.read(eeprom_position));
  Serial.print("\n");
  eeprom_position++;
  delay(100);
}
