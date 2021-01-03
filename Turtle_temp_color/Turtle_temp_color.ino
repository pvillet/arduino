//This example implements a simple sliding On/Off button. The example
// demonstrates drawing and touch operations.
//
//Thanks to Adafruit forums member Asteroid for the original sketch!
//
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ts = Adafruit_FT6206();

#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define SD_CS 4 // Card select for shield use
#define EN_BACKLIGHT 5

boolean RecordOn = false;

#define FRAME_X 210
#define FRAME_Y 180
#define FRAME_W 100
#define FRAME_H 50

#define REDBUTTON_X FRAME_X
#define REDBUTTON_Y FRAME_Y
#define REDBUTTON_W (FRAME_W/2)
#define REDBUTTON_H FRAME_H

#define GREENBUTTON_X (REDBUTTON_X + REDBUTTON_W)
#define GREENBUTTON_Y FRAME_Y
#define GREENBUTTON_W (FRAME_W/2)
#define GREENBUTTON_H FRAME_H

//#define timeInterval 600000 // 10 min
#define timeInterval 5000 // 5s

float Current_Temp = 20.00;
float TempMoyenne = 0.00;
float Temp_Min = -273.00;
float Temp_Max = -273.00;
char tbuffer[7];
char line[7];
unsigned long previousTime = 0;
unsigned long currentTime;
unsigned long BCK_ON_Time;
unsigned int boucle = 0;
#define INDEX 30
float TableauTemperature[INDEX];


void InitialiserTableau()
{
   for (int i=0; i < INDEX; i++) {
      TableauTemperature[i] = -273.00;
   }
}

void CalculerMoyenne()
{
  int id = 0;
  float Somme = 0.00;
   for (int i=0; i < INDEX; i++) {
      if ( TableauTemperature[i] > -273.00) {
         id++;
         Somme = Somme + TableauTemperature[i];
      }
   }
   TempMoyenne = Somme/id;
}


void setup(void)
{
  pinMode(EN_BACKLIGHT, OUTPUT);
  digitalWrite(EN_BACKLIGHT, HIGH);

  Serial.begin(9600);
  Wire.begin();
  // Start up the library
  sensors.begin();  

  tft.begin();
  if (!ts.begin(40)) { 
    Serial.println("Unable to start touchscreen.");
  } 
  else { 
    Serial.println("Touchscreen started."); 
  }

  tft.fillScreen(ILI9341_BLACK);
  // origin = left,top landscape (USB left upper)
  tft.setRotation(1); 

//  Serial.print(F("Initializing SD card..."));
//  if (!SD.begin(SD_CS)) {
//    Serial.println(F("failed!"));
//    tft.setCursor(10 , 100);
//    tft.setTextColor(ILI9341_RED);
//    tft.setTextSize(2);
//    tft.fillScreen(ILI9341_BLACK);
//    tft.println("!! microSD Error !!");
//    while (1) {}
//    return;
//  }
  Serial.println(F("OK!"));
  InitialiserTableau();
}

void loop()
{
 currentTime = millis();

 

if ( (currentTime - previousTime > timeInterval) || (previousTime == 0) ) {
  //Serial.print("boucle: ");  
  //Serial.println(boucle);  
  if (boucle == (INDEX-1))
     boucle = 0;
  else 
     boucle++;
   
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  // Serial.print("Requesting temperatures...");
   sensors.requestTemperatures(); // Send the command to get temperatures
   //Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  //Serial.print("Temperature for the device 1 (index 0) is: ");
  Current_Temp = sensors.getTempCByIndex(0);
  //Serial.println(Current_Temp);  

  if (Current_Temp > -127 && Current_Temp < 80) { // To remove read temp error
  TableauTemperature[boucle] = Current_Temp;
  CalculerMoyenne();  
  Serial.print("boucle: ");  
  Serial.println(boucle);  
  Serial.print("Temperature moyenne: ");  
  Serial.println(TempMoyenne);  
  dtostrf(TempMoyenne,4,2,tbuffer);
  sprintf(line, "%s", tbuffer);
  tft.setCursor(70 , 50);
  tft.setTextSize(6);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_GREEN);
  if (TempMoyenne < 26 || TempMoyenne > 30) {
    tft.setTextColor(ILI9341_RED);
  }
  tft.println(line);

  if (TempMoyenne < Temp_Min || Temp_Min == -273) {
    Temp_Min = TempMoyenne;
  }
  if (TempMoyenne > Temp_Max || Temp_Max == -273) {
    Temp_Max = TempMoyenne;
  }
 
  dtostrf(Temp_Min,4,2,tbuffer);
  sprintf(line, "Min: %s", tbuffer);
  tft.setCursor(10 , 150);
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(line);
  
  dtostrf(Temp_Max,4,2,tbuffer);
  sprintf(line, "Max: %s", tbuffer);
  tft.setCursor(10 , 200);
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(line);

  //tft.drawCircle(200, 100, 5, ILI9341_WHITE);
  }
previousTime = currentTime;
}

  if (ts.touched()) {
   Temp_Max = -273;
   Temp_Min = -273;
   tft.fillScreen(ILI9341_BLUE);
 }

 // On ecrit dans la SD card seulement toute les 1 min
//if ( (currentTime - previousTime > timeInterval) || (previousTime == 0) ) {
//   //EcrireDansFichier();
//   previousTime = currentTime;
//  }

// delay(30000); //30 sec
 // digitalWrite(EN_BACKLIGHT, LOW);

  
//delay(10000);
}
