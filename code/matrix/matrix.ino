#include <MegunoLink.h>
#include <CommandHandler.h>
#include <ArduinoTimer.h>

#include <FastLED.h>
#include <math.h>
#include <string.h>
#include <Adafruit_MCP3008.h>

#include <MegunoLink.h>
#include <filter.h>


// Pin definitions (fixed)
const int LED_DATA_PIN = 8;
const int IR_LED[8] = {6, 12, 9, 4, 18, 2, 19, 3};
const int ADC0_CS_PIN = 10;
const int ADC1_CS_PIN = 11;
const int EDGE_CLK = 7;
const int EDGE_DATA [2] = {19, 18}; // left, right
int IRVals[16]; // 10 bit range
int IRHistory[50][16];
int IRThresholds[16]; //threshold value for each IR

const int NUM_LEDS = 16;
const int BRIGHTNESS = 255;
CRGB leds[NUM_LEDS];

long FilterWeight = 5;
ExponentialFilter<long> ADCFilter(FilterWeight, 0);

enum mode {
  toggle = 0,
  distanceColor = 1,
  distanceBrightness = 2,
  distanceColorBrightness = 3,
  heatMap = 4
};

mode MoveMode = distanceColor;

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
CHSV avgOfNeighbors(int x, int y) {
    int size = sqrt(NUM_LEDS), sum_r = 0, sum_g = 0, sum_b = 0, count = 0;
    int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    
    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
            sum_r += leds[indexOf(nx, ny)][0] * 255 * 0.3;
            sum_g += leds[indexOf(nx, ny)][1] * 255 * 0.3;
            sum_b += leds[indexOf(nx, ny)][2] * 255 * 0.3;
            count++;
        }
    }
    //Serial.println(sum);
    return count > 0 ? CHSV(sum_r / count, sum_g / count, sum_b / count) : CHSV(0, 0, 0);
}

//uses history to return average value of IR at position j
int getAvgOf(int j){
  int s = 0;
  for (int i = 0; i<50;i++){
    s += IRHistory[i][j];
  }  

  return s/50;
}

//shifts all rows and leaves 0 empty for new data, and adds latest new IR DATA into it
void updateHistory(){
  for (int i = 1; i < 50; i++){
    for (int j = 0; j < NUM_LEDS; j++){
      IRHistory[i][j] = IRHistory[i-1][j];
    }
  }
  for (int j = 0; j < NUM_LEDS; j++){
    IRHistory[0][j] = IRVals[j];
  }
}

void calibrateIRs(){
  for (int i = 0; i < 50; i++){
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

    leds[i] = avgOfNeighbors(x, y);
  }
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

unsigned long hueTimer = 0;

void loop() { 
  static uint8_t hue = 0;

  updateHistory();
  //convFilterLeds();

  ADCFilter.Filter(IRVals[0]); //use first LED

  int v = ADCFilter.Current();
  if (v > IRThresholds[0]){
    int h = constrain(map(v, IRThresholds[0], 1023, 0, 255), 0, 255);
    leds[0] = CHSV(h, 255, BRIGHTNESS);
  }

	// for(int i = 0; i < NUM_LEDS; i++) {
  //   //IRS are linear

  //   //hue timer
  //   if (millis() - hueTimer > 20) {
  //       hueTimer = millis();
  //       hue++;
  //     }

  //   int index = i;
  //     if (i % 8 >= 4) {
  //       index = i > 11 ? 15 - i % 12 : 7 - i % 4;
  //   }

  //   if (IRVals[i] > IRThresholds[i]) {
  //     //Serial.println(IRVals[i]);
  //     // Account for LED sequence
  //     if (MoveMode == toggle){
  //       leds[index] = CHSV(hue, 255, BRIGHTNESS);
  //     } else if (MoveMode == distanceColor){
  //       int val = getAvgOf(i);
  //       int h = constrain(map(val, IRThresholds[i], 1023, 0, 255), 0, 255);
  //       //Serial.println(val);
  //       leds[index] = CHSV(hue + h, 255, BRIGHTNESS);
  //     } else if (MoveMode == distanceBrightness){
  //       int val = getAvgOf(i);
  //       int b = constrain(map(val, IRThresholds[i], 1023, 0, 255), 0, 255);
  //       leds[index] = CHSV(255, 255, b);
  //     } else if (MoveMode == distanceColorBrightness){
  //       int val = getAvgOf(i);
  //       int b = constrain(map(val, 1.25*IRThresholds[i], 1023, 0, 255), 0, 255);
  //       //Serial.println(b);
  //       leds[index] = CHSV(b, 255, b);
  //     }
  //   }

  //   if (MoveMode == heatMap){
  //       int val = getAvgOf(i);
  //       int c = map(val, 1.25*IRThresholds[i], 1023, 255, 165);
  //       leds[index] = CHSV(c, 255, BRIGHTNESS);
  //   }
	// }

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