
#include <LCD16x2.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

LCD16x2 lcd;

int buttons;

void setup(){
  Wire.begin();

  lcd.lcdClear();
  
  lcd.lcdGoToXY(2,1);
  lcd.lcdWrite("BUT1:");
  
  lcd.lcdGoToXY(10,1);
  lcd.lcdWrite("BUT2:");
  
  lcd.lcdGoToXY(2,2);
  lcd.lcdWrite("BUT3:");
  
  lcd.lcdGoToXY(10,2);
  lcd.lcdWrite("BUT4:");

  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();  
}
void loop(){
  
  buttons = lcd.readButtons();
  
  lcd.lcdGoToXY(7,1);
  if(buttons & 0x01)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");
    
  lcd.lcdGoToXY(15,1);
  if(buttons & 0x02)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");
    
  lcd.lcdGoToXY(7,2);
  if(buttons & 0x04)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");
    
  lcd.lcdGoToXY(15,2);
  if(buttons & 0x08)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");

  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  Serial.print("Temperature for the device 1 (index 0) is: ");
  Serial.println(sensors.getTempCByIndex(0));  
    
  delay(100);

}
