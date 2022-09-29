#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

String deviceRX = "SensDev", sens1 = "S1 = ", sens2 = "S2 = ";
String LoRaData, sen1, sen2;
int sensor1, sensor2;

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(915E6)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData); 
      int indexS1 = LoRaData.indexOf(sens1);
      int indexS2 = LoRaData.indexOf(sens2);
      int indexComma = LoRaData.indexOf(",");
  
      sen1 = LoRaData.substring(indexS1 + sens1.length(),indexComma);
      sen2 = LoRaData.substring(indexS2 + sens2.length(),indexS2 + sens2.length()+ 3);
    }
    


    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    Serial.printf("sensor1 = %s , sensor2 = %s \n",sen1,sen2);
  }
}