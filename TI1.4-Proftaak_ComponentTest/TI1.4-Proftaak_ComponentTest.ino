//Borden beheerder URL: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
 
// Set the LCD address, number of columns and rows
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change 0x27 to your LCD address if different
const int button1Pin = 32;
const int button2Pin = 33;
const int joystickPin = 4;
const int joystick_x_pin = 14; 
const int joystick_y_pin = 27;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  // Initialize the LCD
  lcd.init();
 
  // Turn on the backlight (optional)
  lcd.backlight();
  lcd.clear();

  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(joystickPin, INPUT_PULLUP);
}
 
void loop() {
  // Read joystick values
  int x_adc_val, y_adc_val; 
  float x_volt, y_volt;
  x_adc_val = analogRead(joystick_x_pin); 
  y_adc_val = analogRead(joystick_y_pin);
  x_volt = ((x_adc_val * 3.3)/ 4095);  //Convert digital value to voltage//
  y_volt = ((y_adc_val * 3.3)/ 4095);  //Convert digital value to voltage//

  // Print button & joystick states
  if (digitalRead(button1Pin) == HIGH && digitalRead(button2Pin) == HIGH){
    lcd.setCursor(0, 0);
    lcd.print("No buttons!   ");
  } else if (digitalRead(button1Pin) == LOW && digitalRead(button2Pin) == LOW){
    lcd.setCursor(0, 0);
    lcd.print("Both buttons! ");
  } else if (digitalRead(button1Pin) == LOW && digitalRead(button2Pin) == HIGH){
    lcd.setCursor(0, 0);
    lcd.print("Button 1 push!");
  } else if (digitalRead(button1Pin) == HIGH && digitalRead(button2Pin) == LOW){
    lcd.setCursor(0, 0);
    lcd.print("Button 2 push!");
  }

  if (digitalRead(joystickPin) == LOW){
    lcd.setCursor(0, 1);
    lcd.print("Joystick push!  ");
  } else if (digitalRead(joystickPin) == HIGH){
    lcd.setCursor(0, 1);
    lcd.print("X: " + String(x_volt) + " Y:  " + String(y_volt));
  }
  delay(100);
}