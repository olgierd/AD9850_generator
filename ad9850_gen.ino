#define W_CLK 12     // connect to AD9850 module word load clock pin (CLK)
#define FQ_UD 8      // connect to freq update pin (FQ)
#define DATA 10      // connect to serial data load pin (DATA)
#define RESET 9      // connect to reset pin (RST).

#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }

void tfr_byte(byte data)
{
  for (int i = 0; i < 8; i++, data >>= 1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);
  }
}

void sendFrequency(uint32_t frequency) {

  uint32_t d = frequency * 34 + (frequency * 35) / 100 + (frequency * 97) / 10000 + (frequency * 38) / 1000000;

  for (int b = 0; b < 4; b++, d >>= 8) {
    tfr_byte(d & 0xFF);
  }
  tfr_byte(0x000);

  pulseHigh(FQ_UD);

}

char key;
int32_t f = 14000000;

void setup() {
  pinMode(FQ_UD, OUTPUT);
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);

  pulseHigh(RESET);
  pulseHigh(W_CLK);
  pulseHigh(FQ_UD);

  Serial.begin(9600);

  sendFrequency(f);
}

void printQRG() {
  Serial.println();
  sendFrequency(f);
  Serial.print(f);
}

void loop() {

  while(Serial.available() == 0) { }

  key = Serial.read();

  if (key == '`') {
    f = 0;
    Serial.println();
    Serial.println("QRG: ");
    while (1) {
      while(Serial.available() == 0) { }; key = Serial.read();
      if (key < '0' or key > '9') break;
      Serial.print(key);
      f = f * 10 + (uint8_t)(key - '0');
    }
  }

  if (key == 'x') f -= 10;
  if (key == 'c') f += 10;
  if (key == 's') f -= 100;
  if (key == 'd') f += 100;
  if (key == 'w') f -= 1000;
  if (key == 'e') f += 1000;
  if (key == '2') f -= 10000;
  if (key == '3') f += 10000;
  if (key == '1') f -= 100000;
  if (key == '4') f += 100000;

  printQRG();

}
