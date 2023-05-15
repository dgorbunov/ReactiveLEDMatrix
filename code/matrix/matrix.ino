#include <FastLED.h>
#include <math.h>
#include <string.h>
#include <Adafruit_MCP3008.h>

// Pin definitions (fixed)
const int LED_DATA_PIN = 8;
const int IR_LED[8] = {6, 12, 9, 4, 18, 2, 19, 3};
const int ADC0_CS_PIN = 10;
const int ADC1_CS_PIN = 11;
const int EDGE_CLK = 7;
const int EDGE_DATA [2] = {19, 18}; // left, right
int IRVals[16]; // 10 bit range
int IRHistory[5][16];
int IRThresholds[16]; //threshold value for each IR

const int NUM_LEDS = 16;
const int BRIGHTNESS = 100;
CRGB leds[NUM_LEDS];

enum mode {
  toggle = 0,
  distanceColor = 1,
  distanceBrightness = 2
};

mode MoveMode = distanceBrightness;

Adafruit_MCP3008 adc0;
Adafruit_MCP3008 adc1;

int indexOf(int x, int y){
  int i;
  // reverse
  if (y % 2 == 1){
    i = int(y * sqrt(NUM_LEDS) + x);
  } else {
    i = int(4*y) - (sqrt(NUM_LEDS) + x);
  }

  return i;
}

//converts index of led to position
int positionOf(int i, int* x, int* y){
  if (i < 4) {
    *x = i;
    *y = 0;
  } else if (i < 8) {
    *x = 8 - i;
    *y = 1;
  } else if (i < 12) {
    *x = (8 + i) % 4;
    *y = 2;
  } else {
    *x = (15 - i) %4;
    *y = 3;
  }
}

// gets avg of all valid neighbors (out of 8 directions)
CRGB avgDifOfNeighbors(int x, int y, int mult) {
    int size = sqrt(NUM_LEDS), sum_r = 0, sum_g = 0, sum_b = 0, count = 0;
    int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    
    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
            sum_r += leds[indexOf(nx, ny)][0] * 255 * mult;
            sum_g += leds[indexOf(nx, ny)][1] * 255 * mult;
            sum_b += leds[indexOf(nx, ny)][1] * 255 * mult;
            count++;
        }
    }
    //Serial.println(sum);
    return count > 0 ? CRGB(sum_r / count, sum_g / count, sum_b / count) : CRGB(0, 0, 0);
}

//uses history to return average value of IR at position j
int getAvgOf(int j){
  int s = 0;
  for (int i = 0; i<5;i++){
    s += IRHistory[i][j];
  }  

  return s/5;
}

//shifts all rows and leaves 0 empty for new data, and adds latest new IR DATA into it
void updateHistory(){
  for (int i = 1; i < 5; i++){
    for (int j = 0; j < NUM_LEDS; j++){
      IRHistory[i][j] = IRHistory[i-1][j];
    }
  }
  for (int j = 0; j < NUM_LEDS; j++){
    IRHistory[0][j] = IRVals[j];
  }
}

void calibrateIRs(){
  for (int i = 0; i < 10; i++){
    for (int i = 0; i < 4; i ++) {
      captureIR(i, 0);
    }

    for (int i = 0; i < 4; i ++) {
      captureIR(i, 1);
    }

    updateHistory();
  }

  Serial.println("Calibration Results");
  for (int i = 0; i < NUM_LEDS; i++){
    IRThresholds[i] = getAvgOf(i) + 100;
    Serial.print(i);
    Serial.print(", thresh: ");
    Serial.print(IRThresholds[i]);
  }
}

void setup() { 
	Serial.begin(115200);

  for (int i = 0; i < 8; i++) {
    pinMode(IR_LED[i], OUTPUT);
  }
  
	FastLED.addLeds<WS2812B,LED_DATA_PIN,RGB>(leds, NUM_LEDS);
	FastLED.setBrightness(BRIGHTNESS);

  adc0.begin(ADC0_CS_PIN);
  adc1.begin(ADC1_CS_PIN);

  calibrateIRs();
}

// modifies in place the LED matrix to smooth the data
void convFilterLeds(){
  for (int i = 0; i < NUM_LEDS; i++){
    //assuming in linear order
    int x, y;
    positionOf(i, &x, &y);
    Serial.print("position of LED ");
    Serial.print(i);
    Serial.print(" ");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.println();

    leds[i] = avgDifOfNeighbors(x, y, 0.3);
  }
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

unsigned long hueTimer = 0;

void loop() { 
  static uint8_t hue = 0;

  updateHistory();

	for(int i = 0; i < NUM_LEDS; i++) {
    //IRS are linear
    if (IRVals[i] > IRThresholds[i]) {
      //Serial.println(IRVals[i]);
      // Account for LED sequence
      int index = i;
      if (i % 8 >= 4) {
        index = i > 11 ? 15 - i % 12 : 7 - i % 4;
      }

      if (millis() - hueTimer > 10) {
        hueTimer = millis();
        hue++;
      }

      if (MoveMode == toggle){
        leds[index] = CHSV(hue, 255, BRIGHTNESS);
      } else if (MoveMode == distanceColor){
        int val = getAvgOf(i);
        int h = (val - IRVals[i]) * (val - IRVals[i]);
        //Serial.println(val);
        leds[index] = CHSV(val, 255, BRIGHTNESS);
      } else if (MoveMode == distanceBrightness){
        int val = getAvgOf(i);
        int b = (val - IRVals[i] - 100); //* (val - IRVals[i]);
        Serial.println(b);
        leds[index] = CHSV(255, 255, b);
      }
      
    }
	}

  delayMicroseconds(5);
  //convFilterLeds();
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
  delayMicroseconds(10);
  IRVals[index * 2 + offset] = readADC(index, adc);
  IRVals[index * 2 + 1 + offset] = readADC(index + 1, adc);
  delayMicroseconds(10);
  digitalWrite(IR_LED[index + adc], LOW);
}

int readADC(int index, int adc) {
  return adc == 1 ? adc1.readADC(7 - index) : adc0.readADC(index);
}