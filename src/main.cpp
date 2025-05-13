/*************************************************** 
  This is an example for the Si4713 FM Radio Transmitter with RDS

  Designed specifically to work with the Si4713 breakout in the
  adafruit shop
  ----> https://www.adafruit.com/products/1958

  These transmitters use I2C to communicate, plus reset pin. 
  3 pins are required to interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution

  Many thx to https://github.com/phrm/fmtx/blob/master/firmware/firmware.ino !

 ****************************************************/

#include <Wire.h>
#include <Adafruit_Si4713.h>
#include <main.h>
#define _BV(n) (1 << n)

#define RESETPIN 12

#define FMSTATION 9990      // 10230 == 102.30 MHz

Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit Radio - Si4713 Test"); 
  
  Wire.begin(0x63);

  init_fm();

  if (! radio.begin()) {  // begin with address 0x63 (CS high default)
    Serial.println("Couldn't find radio?");
    while (1);
  }

  // Uncomment to scan power of entire range from 87.5 to 108.0 MHz
  /*
  for (uint16_t f  = 8750; f<10800; f+=10) {
   radio.readTuneMeasure(f);
   Serial.print("Measuring "); Serial.print(f); Serial.print("...");
   radio.readTuneStatus();
   Serial.println(radio.currNoiseLevel);
   }

  */

  delay(250);
   

  Serial.print("\nSet TX power");
  radio.setTXpower(115);  // dBuV, 88-115 max

  Serial.print("\nTuning into "); 
  Serial.print(FMSTATION/100); 
  Serial.print('.'); 
  Serial.println(FMSTATION % 100);
  radio.tuneFM(FMSTATION); // 102.3 mhz

  // This will tell you the status in case you want to read it from the chip
  radio.readTuneStatus();
  Serial.print("\tCurr freq: "); 
  Serial.println(radio.currFreq);
  Serial.print("\tCurr freqdBuV:"); 
  Serial.println(radio.currdBuV);
  Serial.print("\tCurr ANTcap:"); 
  Serial.println(radio.currAntCap);

  // begin the RDS/RDBS transmission
  radio.beginRDS();
  radio.setRDSstation("AdaRadio");
  radio.setRDSbuffer( "Adafruit g0th Radio!");

  Serial.println("RDS on!");  

  radio.setGPIOctrl(_BV(1) | _BV(2));  // set GP1 and GP2 to output
}

void init_fm() {
  // reset board using pin
  pinMode(RESETPIN, OUTPUT);
  digitalWrite(RESETPIN, LOW);
  delay(100);
  digitalWrite(RESETPIN, HIGH);
  delay(100);

  byte command[3] = {0x01, 0x12, 0x50};
  byte reply[1];

  Wire.beginTransmission(0x63);
  for (int i = 0; i < 3; i++) {
    Wire.write(command[i]);
    Serial.print("0x");
    Serial.println(command[i], HEX);
  }
  Wire.endTransmission();

  delay(10);

  // request 1-byte reply
  Wire.requestFrom(0x63, 1);
  
  int i = 0;
  while (Wire.available() && i < 1) {
    reply[i++] = Wire.read();
  }

  // print received data
  Serial.print("Received: ");
  for (int j = 0; j < i; j++) {
    Serial.print("0x");
    Serial.print(reply[j], HEX);
    Serial.print(" ");
  }
  Serial.println();

  delay(100);
  
  byte command2[1] = {0x10}; // get revision
  byte reply2[9];

  Wire.beginTransmission(0x63);
  for (int i = 0; i < 1; i++) {
    Wire.write(command2[i]);
    Serial.print("0x");
    Serial.println(command2[i], HEX);
  }
  Wire.endTransmission();

  delay(10);

    // request 9-byte reply
  Wire.requestFrom(0x63, 9);
  
  int i2 = 0;
  while (Wire.available() && i2 < 9) {
    reply2[i2++] = Wire.read();
  }

  // print received data
  Serial.print("Received: ");
  for (int j = 0; j < i2; j++) {
    Serial.print("0x");
    Serial.print(reply2[j], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
  radio.readASQ();
  Serial.print("\tCurr ASQ: 0x"); 
  Serial.println(radio.currASQ, HEX);
  Serial.print("\tCurr InLevel:"); 
  Serial.println(radio.currInLevel);
  Serial.print("\tCurr ANTcap:"); 
  Serial.println(radio.currAntCap);
  // toggle GPO1 and GPO2
  radio.setGPIO(_BV(1));
  delay(500);
  radio.setGPIO(_BV(2));
  delay(500);
}
