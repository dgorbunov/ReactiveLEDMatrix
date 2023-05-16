#include <FirebaseESP32.h>
#include <WiFi.h>
#include <SoftwareSerial.h>
#include "config.h"

#define TX_PIN 13

FirebaseData firebaseData;
EspSoftwareSerial::UART SWSerial;

int previousMode = -1;

void setup() {
  Serial.begin(9600);
  SWSerial.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, TX_PIN, TX_PIN, false, 256);
  SWSerial.enableIntTx(false);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected: ") ;
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);     
}

void loop() {
  if (Firebase.getInt(firebaseData, "/Led1Mode")) {
    int data = firebaseData.intData();
    if (data != previousMode) {
      previousMode = data;
      Serial.println("Sending: " + (String)data);
      sendSerial(&SWSerial, (byte)data);
    }
  }
}

void sendSerial(EspSoftwareSerial::UART* ss, byte payload) {
  ss->enableTx(true);
  ss->write(payload);
  ss->enableTx(false);
}