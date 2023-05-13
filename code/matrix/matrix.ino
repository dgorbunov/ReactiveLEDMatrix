#include <FastLED.h>
#include <math.h>
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

//macro for getting index of x, y position
#define XY(x, y) ((y) * sqrt(NUM_LEDS) + (x))

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

// gets avg of all valid neighbors (out of 8 directions)
int avgOfNeighbors(int x, int y) {
    int size = sqrt(NUM_LEDS), sum = 0, count = 0;
    int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    
    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
            sum += leds[XY(nx, ny)];
            count++;
        }
    }
    return count > 0 ? sum / count : 0;
}


//converts index of led to position
int positionOf(int i, int* x, int* y){
  int size = sqrt(NUM_LEDS);
  *x = i % size;
  *y = i / size;
}

// modifies in place the LED matrix to smooth the data
void convFilterLeds(){
  for (int i = 0; i < NUM_LEDS; i++){
    //assuming in linear order
    int x, y;
    positionOf(i, &x, &y);
    printf("position of LED %d is (%d, %d)\n", i, x, y);

    leds[i] = avgOfNeighbors(x, y);
  }
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