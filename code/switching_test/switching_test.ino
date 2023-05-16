const int IR_LED[8] = {6, 12, 9, 4, 18, 2, 19, 3};

void setup() {
  for (int i = 0; i < 16; i++) {
    pinMode(IR_LED[i], OUTPUT);
  }
}

void loop() {
  for (int i = 0; i < 4; i++) {
    captureIR(i, 0);
    captureIR(i, 1);
  }
}

void captureIR(int index, int adc) {
  index *= 2;
  int offset = 2 * adc;

  digitalWrite(IR_LED[index + adc], LOW); 
  digitalWrite(IR_LED[(index + adc + 1) % 8], HIGH);

  delay(500);
}