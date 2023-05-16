#include <Adafruit_NeoPixel.h>
#include <FirebaseESP32.h>
#include <WiFi.h>
#include "config.h" 

#define WIFI_SSID "iPhone" // your wifi SSID
#define WIFI_PASSWORD "deeznutz" //your wifi PASSWORD

#define LedPin 25         // pin d0 as toggle pin
#define NUM_PIXELS 16      //number of rgb led
#define PIN_PIXELS LedPin     //pin d5 as neopixel input
FirebaseData firebaseData;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN_PIXELS, NEO_GRB + NEO_KHZ800);
void setup (){
  pinMode(LedPin, OUTPUT);
  pixels.begin();
  Serial.begin(9600);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ") ;
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);     
}

// Function to set the LED to wave mode
void waveMode() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  }
  pixels.show();
  delay(100);
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
  delay(100);
}

// Function to set the LED to party mode
void partyMode() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(random(255), random(255), random(255)));
  }
  pixels.show();
  delay(100);
}

void rainbowMode() {
  int wait = 20;
  for (int j = 0; j < 256; j++) {
    for (int i = 0; i < NUM_PIXELS; i++) {
      pixels.setPixelColor(i, wheel((i + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void linearMode() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(random(255), random(255), random(255)));
  }
  pixels.show();
  delay(100);
}

void starMode() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(random(255), random(255), random(255)));
  }
  pixels.show();
  delay(100);
}

void loop (){
  if (Firebase.getString(firebaseData, "/Led1Mode")) {
        String mode = firebaseData.stringData();
        Serial.print(mode);
        if (mode == "0") {
          // set the LED to rainbow mode
          rainbowMode();
        } else if (mode == "1") {
          // set the LED to wave mode
          waveMode();
        } else if (mode == "2") {
          // set the LED to party mode
          partyMode();
        } 
        else if (mode == "3") {
          //set the LED to linear
          linearMode();
        }
        else if (mode == "4") {
          //set the LED to linear
          linearMode();
        }
      }

  }