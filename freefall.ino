//for bmp280 control
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

//for sd card R/W
#include <SD.h>

#include <CurieIMU.h>
#include <MadgwickAHRS.h>
#include <math.h>


#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10

const int chipSelect = 10;

Adafruit_BMP280 bme; // I2C
//Adafruit_BMP280 bme(BMP_CS); // hardware SPI
//Adafruit_BMP280 bme(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

File myFile;
Madgwick filter;
Sd2Card card;
unsigned long microsPerReading, microsPrevious;
float accelScale, gyroScale;
int freeFallCounter;
float current, pre;
float heightBuffer[20];
float heightPre, heightCur;
int lc;

void setup() {
  //tone(3, 5000);
  Serial.begin(9600);
 //digitalWrite(0, HIGH);
  // start the IMU and filter
  CurieIMU.begin();
  CurieIMU.setGyroRate(25);
  CurieIMU.setAccelerometerRate(25);
  filter.begin(25);

  // Set the accelerometer range to 2G
  CurieIMU.setAccelerometerRange(2);
  // Set the gyroscope range to 250 degrees/second
  CurieIMU.setGyroRange(250);

  // initialize variables to pace updates to correct rate
  microsPerReading = 1000000 / 25;
  microsPrevious = micros();
  freeFallCounter = 0;
  current = 1.0;
  lc = 0;

  //for bmp280
  if (!bme.begin()) {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
  pinMode(7, OUTPUT);
  //intial sd card
  /*pinMode(10, OUTPUT);
 
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }*/
  
  // if the file opened okay, write to it:
  /*if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
  // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }*/
  
}

void loop() {
  int aix, aiy, aiz;
  int gix, giy, giz;
  float ax, ay, az;
  float gx, gy, gz;
  float roll, pitch, heading;
  unsigned long microsNow;
  //int filtered;

  // check if it's time to read data and update the filter
  microsNow = micros();
  if (microsNow - microsPrevious >= microsPerReading) {

    // read raw data from CurieIMU
    CurieIMU.readMotionSensor(aix, aiy, aiz, gix, giy, giz);

    // convert from raw data to gravity and degrees/second units
    ax = convertRawAcceleration(aix);
    ay = convertRawAcceleration(aiy);
    az = convertRawAcceleration(aiz);
    gx = convertRawGyro(gix);
    gy = convertRawGyro(giy);
    gz = convertRawGyro(giz);

    // update the filter, which computes orientation
    filter.updateIMU(gx, gy, gz, ax, ay, az);

    // print the heading, pitch and roll
      roll = filter.getRoll();
      pitch = filter.getPitch();
      heading = filter.getYaw();
      /*Serial.print("Orientation: ");
      Serial.print(heading);
      Serial.print(" ");
      Serial.print(pitch);
      Serial.print(" ");
      Serial.println(roll);*/
      
      /*Serial.print("ax is ");
      Serial.print(ax);
      Serial.print(" ay is ");
      Serial.print(ay);
      Serial.print(" az is ");
      Serial.println(az);
      delay(500);*/
      
    pre = current;
    current = sqrt(pow(ax, 2) + pow(ay, 2) + pow(az, 2));
    current = 0.7f * current + 0.3f * pre;//cuurent just a simple filter, will be improved in future
    //heightBuffer[lc%20 = bme.readAltitude(1013.25); //this should be adjusted to your local forcase
    heightPre = heightCur;
    heightCur = bme.readAltitude(1013.25); 
    heightCur = 0.7f*heightCur + 0.3f*heightPre;
    //Serial.print("height is ");
    //Serial.println(heightCur);
    Serial.println(lc);
    if (current < 0.65) {
      freeFallCounter++;
    }
    else if(freeFallCounter>8){
      freeFallCounter = 0;
    }
    if (freeFallCounter > 10 && heightCur < heightPre) {
        Serial.print("current accelaration is ");
        Serial.println(0.98f - current);
        Serial.println("free fall detected");
        tone(3, 5000);   // turn the buzzer on
        freeFallCounter = 0;
    }

    

    if(heightPre-heightCur>1.0f){
        digitalWrite(0, HIGH);   // turn the LED on (HIGH is the voltage level)
        Serial.print("height is ");
        Serial.println(heightCur - heightPre);
        Serial.println("free fall detected");
    }
    if(abs(pitch) > 60 || abs(roll)>60){
        tone(3, 3500);   // turn the buzzer on
        digitalWrite(7, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);
        digitalWrite(7,LOW);
        delay(1000);
        
        
        Serial.println("tilt detected");
    }
    // increment previous time, so we keep proper pace
    microsPrevious = microsPrevious + microsPerReading;
  }
  lc++;
  delay(1);//freqz at 1000Hz
}

float convertRawAcceleration(int aRaw) {
  // since we are using 2G range
  // -2g maps to a raw value of -32768
  // +2g maps to a raw value of 32767

  float a = (aRaw * 2.0) / 32768.0;
  return a;
}

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range
  // -250 maps to a raw value of -32768
  // +250 maps to a raw value of 32767

  float g = (gRaw * 250.0) / 32768.0;
  return g;
}
