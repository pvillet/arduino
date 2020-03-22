
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
int backlight_step = 30;
int backlight = 128;
char tbuffer[7];
char line[7];
float Current_Temp = 20.00;
float Max_Temp = -50.00;
float Min_Temp = 100.00;


void setup(){
  Wire.begin();

  lcd.lcdSetBlacklight(backlight);

  lcd.lcdClear();
  
//  lcd.lcdGoToXY(2,1);
//  lcd.lcdWrite("BUT1:");
//  
//  lcd.lcdGoToXY(10,1);
//  lcd.lcdWrite("BUT2:");
//  
//  lcd.lcdGoToXY(2,2);
//  lcd.lcdWrite("BUT3:");
//  
//  lcd.lcdGoToXY(10,2);
//  lcd.lcdWrite("BUT4:");

  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();  
}
void loop(){

for (int l = 0; l <100; l++)  {
  buttons = lcd.readButtons();
  if( buttons != 15) {
     //Serial.println("Appui detecte");    
     if( buttons == 14) {
        Max_Temp = -50.00;
        Min_Temp = 100.00;
     }
     if( buttons == 7) {
        backlight = backlight + backlight_step;
        }
     if( buttons == 11) {
        backlight = backlight - backlight_step;
     }
     break;
  }
   delay(10);
}

  if (backlight > 255)
         backlight = 255;
  if (backlight < 0)
         backlight = 0;

  lcd.lcdSetBlacklight(backlight);
  Serial.println(backlight);  

  //buttons = lcd.readButtons();
//  Serial.println("button: ");
//  Serial.println(buttons);
  
//  lcd.lcdGoToXY(7,1);
     
//    
//  lcd.lcdGoToXY(15,1);
//  if(buttons & 0x02)  
//    lcd.lcdWrite("0");
//  else
//    lcd.lcdWrite("1");
//    
//  lcd.lcdGoToXY(7,2);
//  if(buttons & 0x04)  
//    lcd.lcdWrite("0");
//  else
//    lcd.lcdWrite("1");
//    
//  lcd.lcdGoToXY(15,2);
//  if(buttons & 0x08)  
//    lcd.lcdWrite("0");
//  else
//    lcd.lcdWrite("1");

  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
//  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
//  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  Serial.print("Temperature for the device 1 (index 0) is: ");
  Current_Temp = sensors.getTempCByIndex(0);
  Serial.println(Current_Temp);  

  if (Current_Temp > Max_Temp)
     Max_Temp = Current_Temp;
     
  if (Current_Temp < Min_Temp)
     Min_Temp = Current_Temp;

/* Affichage premiere ligne */
  dtostrf(Current_Temp,4,2,tbuffer);
  sprintf(line, "%s C", tbuffer);
  lcd.lcdGoToXY(5,1);
  lcd.lcdWrite(line);

/* Affichage deuxieme ligne */
  dtostrf(Min_Temp,4,2,tbuffer);
  sprintf(line, "%s C", tbuffer);
  lcd.lcdGoToXY(1,2);
  lcd.lcdWrite(line);

  dtostrf(Max_Temp,4,2,tbuffer);
  sprintf(line, "%s C", tbuffer);
  lcd.lcdGoToXY(9,2);
  lcd.lcdWrite(line);
  
//  delay(1000);

}
