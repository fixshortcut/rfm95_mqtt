#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define ss 5
#define rst 14
#define dio0 2

// const char* ssid = "WIDYA ROBOTIC";
// const char* password = "011118WidyaWII";
const char* ssid = "WIDYA ROBOTICS";
const char* password = "011118widya";
// const char* ssid = "KAMEKATRON";
// const char* password = "terserahkamu";
const char* ssid2 = "ndang turu!";
const char* password2 = "ragelemturu";

// const char* mqtt_server = "192.168.111.86"; // mqtt broker ip
// const char* mqtt_server = "192.168.18.85"; //mekatron
const char* mqtt_server = "192.168.18.52"; //widya robotics
const char* mqtt_server2 = "192.168.0.102";

const int id_wifi = 1;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waktuPesan = 0 ;
char msg[50];


//define the pins used by the transceiver module
String deviceRX = "SensDev", sens1 = "S1 = ", sens2 = "S2 = ";
String LoRaData, sen1, sen2;
int sensor1, sensor2;

void callback(char* topic, byte* message, unsigned int length) { //receive data
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      // digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      // digitalWrite(ledPin, LOW);
    }
  }
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("ip :");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
   
  client.setCallback(callback);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp32Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa MQTT");
  Serial.println("Setting up WiFi ...");
  setup_wifi();
  

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
    Serial.print("Lora: ");

    // read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData); 
      int indexS1 = LoRaData.indexOf(sens1);
      int indexS2 = LoRaData.indexOf(sens2);
      int indexComma = LoRaData.indexOf(",");
  
      sen1 = LoRaData.substring(indexS1 + sens1.length(),indexComma);
      sen2 = LoRaData.substring(indexS2 + sens2.length(),indexS2 + sens2.length()+ 3);
      Serial.printf("sensor1 = %s , sensor2 = %s \n",sen1,sen2);
    }
    


    // print RSSI of packet
    // Serial.print("' with RSSI ");
    // Serial.println(LoRa.packetRssi());
    
  }
  
  if(!client.connected()){
    reconnect();
  }
  client.loop();
  if (millis() - waktuPesan >= 5000) {
    waktuPesan = millis();

    const char *strSen1 = sen1.c_str();
    const char *strSen2 = sen2.c_str();
    Serial.println(client.publish("esp32/sensor1", strSen1));
    Serial.println(client.publish("esp32/sensor2", strSen2));

    Serial.print("sensor2 :");
    Serial.print(strSen1);
    Serial.print("| sensor2 : ");
    Serial.println(strSen2);
    
  }
  
}