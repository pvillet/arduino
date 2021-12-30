/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
#include <SigFox.h>
#include <ArduinoLowPower.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

#define UINT16_t_MAX  65536
#define INT16_t_MAX   UINT16_t_MAX/2

int16_t convertoFloatToInt16(float value, long max, long min) {
  float conversionFactor = (float) (INT16_t_MAX) / (float)(max - min);
  return (int16_t)(value * conversionFactor);
}

uint16_t convertoFloatToUInt16(float value, long max, long min = 0) {
  float conversionFactor = (float) (UINT16_t_MAX) / (float)(max - min);
  return (uint16_t)(value * conversionFactor);
}

/*
    ATTENTION - the structure we are going to send MUST
    be declared "packed" otherwise we'll get padding mismatch
    on the sent data - see http://www.catb.org/esr/structure-packing/#_structure_alignment_and_padding
    for more details
*/
typedef struct __attribute__ ((packed)) sigfox_message {
  uint8_t Tpile;
  uint8_t Alim;
  int16_t bmeTemperature1;
  uint16_t bmePressure1;
  uint8_t bmeHumidity1;
  int16_t bmeTemperature2;
  uint16_t bmePressure2;
  uint8_t bmeHumidity2;
} SigfoxMessage;

// stub for message which will be sent
SigfoxMessage msg;

// Set oneshot to false to trigger continuous mode when you finisched setting up the whole flow
int oneshot = false;


Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;

void setup() {
    
  if (oneshot == true) {
    Serial1.begin(115200);
    while (!Serial) {};
  }

  if (oneshot == true) {
    Serial1.println(F("BME280 test"));
  }
    bool status;
    
    // default settings
    // (you can also pass in a Wire library object like &Wire2)
    status = bme.begin();  
    if (!status) {
        Serial1.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    

  if (!SigFox.begin()) {
    Serial1.println("Shield error or not present!");
    return;
  }
  // Enable debug led and disable automatic deep sleep
  // Comment this line when shipping your project :)
  //  if (oneshot == true) {
      SigFox.debug();
  //  }

  String version = SigFox.SigVersion();
  String ID = SigFox.ID();
  String PAC = SigFox.PAC();

   if (oneshot == true) {
 // Display module informations
  Serial1.println("MKRFox1200 Sigfox first configuration");
  Serial1.println("SigFox FW version " + version);
  Serial1.println("ID  = " + ID);
  Serial1.println("PAC = " + PAC);

  Serial1.println("");
   }
  // Send the module to the deepest sleep
  SigFox.end();

   delayTime = 15*60*1000;
   //delayTime = 10*1000;
   
  if (oneshot == true) {
    Serial1.println();
  }

  // To read Battery voltage
  analogReadResolution(12);
  analogReference(AR_INTERNAL1V0);

  // To monitor if 5V is present or not
  pinMode(2, INPUT);
  //pinMode(2, INPUT_PULLUP);
  //LowPower.attachInterruptWakeup(2, alarmEvent2, CHANGE);
  LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, alarmEvent0, CHANGE);

   // weather monitoring
   if (oneshot == true) {
    Serial1.println("-- Weather Station Scenario --");
    Serial1.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
    Serial1.println("filter off");
   }
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF   );
                      
    // suggested rate is 1/60Hz (1m)
    //delayTime = 60000; // in milliseconds
}


void loop() { 

    bme.takeForcedMeasurement(); // has no effect in normal mode
 
    if (oneshot == true) {
     printValues();
    }

  float temperature = bme.readTemperature();
  msg.bmeTemperature1 = int16_t (temperature*10);
  //msg.bmeTemperature = temperature;
  float pressure = bme.readPressure();
  msg.bmePressure1 = uint16_t (pressure/10.0F);
  //msg.bmePressure = (pressure/100.0F);
  float humidity = bme.readHumidity();
  msg.bmeHumidity1 = uint8_t (humidity);
  //msg.bmeHumidity = (humidity);
  
  msg.bmeTemperature2 = int16_t (temperature*10);
  msg.bmePressure2 = uint16_t (pressure/10.0F);
  msg.bmeHumidity2 = uint8_t (humidity);
  
  int canvalue=analogRead(ADC_BATTERY);
  float Vf=(canvalue/4095.0)*(1.0/1.0)*(1.0/(33.0/(68.0+33.0)));
  int Is_5V = digitalRead(2);
  if (Is_5V == 1) {
     msg.Alim = 100;
  }
  else {
    msg.Alim = 0;
  }

     //
  if (Vf < 1.0) {
  msg.Tpile = uint16_t (0);
  }
  else {
  msg.Tpile = uint16_t ((Vf *100)-100);
  }
  
  // Start the module
  SigFox.begin();
  // Wait at least 30ms after first configuration (100ms before)
  delay(100);
  // Clears all pending interrupts
  SigFox.status();
  delay(1);

  SigFox.beginPacket();
  //SigFox.write((uint8_t*)&msg, 9);
  SigFox.write((uint8_t*)&msg, 12);
  SigFox.endPacket();
  SigFox.end();

  // 15*60*1000 = 15 minutes
  for (int lp = 0; lp < 156; lp++) { 
     LowPower.sleep(10000);
     if (Is_5V != digitalRead(2)) {
      //Input Power has changed, send a message
      break;
     }
  }

  //LowPower.sleep(delayTime);
  //delay(delayTime);
}


void printValues() {
    Serial1.print("Temperature = ");
    Serial1.print(bme.readTemperature());
    Serial1.println(" *C");

    Serial1.print("Pressure = ");

    Serial1.print(bme.readPressure() / 100.0F);
    Serial1.println(" hPa");

    Serial1.print("Approx. Altitude = ");
    Serial1.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial1.println(" m");

    Serial1.print("Humidity = ");
    Serial1.print(bme.readHumidity());
    Serial1.println(" %");

    int canvalue=analogRead(ADC_BATTERY);
    float Vf=(canvalue/4095.0)*(1.0/1.0)*(1.0/(33.0/(68.0+33.0)));
    Serial1.print("Tension = ");
    Serial1.print(Vf);
    Serial1.println(" V");

    int Is_5V = digitalRead(2);
    Serial1.print("5V = ");
    Serial1.print(Is_5V);
    Serial1.println();

    Serial1.println();
}

void alarmEvent0() {
 volatile int alarm_source = 0;
    if (oneshot == true) {
    Serial1.println("-- alarmEvent0 --");
   }
}
   
 void alarmEvent2() {
  volatile int alarm_source2 = 0;
    if (oneshot == true) {
    Serial1.println("-- alarmEvent2 --");
   }
}
