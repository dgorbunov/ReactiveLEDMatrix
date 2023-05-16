#include <FirebaseESP32.h>
#include <WiFi.h>
#include "config.h"

#define TX_PIN 13

FirebaseData firebaseData;

int previousMode = -1;

void setup() {
  Serial.begin(9600);
  pinMode(TX_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(TX_PIN, HIGH);

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
      sendPayload(data);
    }
  }
}

void sendPayload(int data) {
  int pulseWidth = 50;
  int intervals = data + 1;

  digitalWrite(LED_BUILTIN, HIGH);

  for (int i = 0; i < intervals; i++) {
    digitalWrite(TX_PIN, LOW);
    delay(pulseWidth);
  }

  digitalWrite(TX_PIN, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
}