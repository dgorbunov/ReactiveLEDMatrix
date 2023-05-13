#include <FastLED.h>
#include <Adafruit_MCP3008.h>

// Pin definitions (fixed)
const int LED_DATA_PIN = 8;
const int IR_LED[8] = {6, 12, 9, 4, 18, 2, 19, 3};
const int ADC0_CS_PIN = 10;
const int ADC1_CS_PIN = 11;
const int EDGE_CLK = 7;
const int EDGE_DATA [2] = {19, 18}; // left, right

const int NUM_LEDS = 16;
CRGB leds[NUM_LEDS];

Adafruit_MCP3008 adc0;
Adafruit_MCP3008 adc1;

int IRVals[16];
// 10 bit range

void setup() { 
	Serial.begin(115200);

  for (int i = 0; i < 8; i++) {
    pinMode(IR_LED[i], OUTPUT);
  }
  
	FastLED.addLeds<WS2812B,LED_DATA_PIN,RGB>(leds, NUM_LEDS);
	FastLED.setBrightness(5);

  adc0.begin(ADC0_CS_PIN);
  adc1.begin(ADC1_CS_PIN);
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void loop() { 
	for(int i = 0; i < NUM_LEDS; i++) {
    if (IRVals[i] > 100) {
      // Account for LED sequence
      int index = i;
      if (i % 8 >= 4) {
        index = i > 11 ? 15 - i % 12 : 7 - i % 4;
      }

      leds[index] = CRGB(255, 255, 255);
    }
	}

  fadeall();
  FastLED.show();


  for (int i = 0; i < 4; i ++) {
    captureIR(i, 0);
  }

  for (int i = 0; i < 4; i ++) {
    captureIR(i, 1);
  }
}

void captureIR(int index, int adc) {
  index *= 2;
  int offset = 2 * adc;
  digitalWrite(IR_LED[index + adc], HIGH); 
  delayMicroseconds(5);
  IRVals[index * 2 + offset] = readADC(index, adc);
  IRVals[index * 2 + 1 + offset] = readADC(index + 1, adc);
  delayMicroseconds(5);
  digitalWrite(IR_LED[index + adc], LOW);
}

int readADC(int index, int adc) {
  return adc == 1 ? adc1.readADC(7 - index) : adc0.readADC(index);
}