#define REV_1_1

#define LED_SDI  D0
#define LED_CLK  D1
#define LED_EN   D2
#define LED_LAT  D3
#define LED_CLR  D4
#define SCAN_0   D5
#define SCAN_1   D6
#define READ_0  D10
#define READ_1   D9
#define READ_2   D8
#define READ_3   D7

const uint8_t font[] = {0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110, 0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110, 0b11101110, 0b00111110, 0b00011010, 0b01111010, 0b10011110, 0b10001110};

const int8_t _scan[2] = {SCAN_0, SCAN_1};
const int8_t _read[4] = {READ_0, READ_1, READ_2, READ_3};


void init_io(void) {
  pinMode(LED_SDI, OUTPUT);
  pinMode(LED_CLK, OUTPUT);
  pinMode(LED_EN , OUTPUT);
  pinMode(LED_LAT, OUTPUT);
  pinMode(LED_CLR, OUTPUT);

  for(int8_t s = 0; s < 2; s++) {
    pinMode(_scan[s], OUTPUT);
    digitalWrite(_scan[s], HIGH);
  }

  for(int8_t r = 0; r < 4; r++) {
    pinMode(_read[r],  INPUT);
  }
}

void clear595(void) {
  digitalWrite(LED_CLR,  LOW);
  digitalWrite(LED_CLR, HIGH);
}

void send595(const uint8_t dat) {
  for(int8_t i = 0; i < 8; i++) {
    digitalWrite(LED_SDI, (dat >> i) & 1);
    digitalWrite(LED_CLK, HIGH);
    digitalWrite(LED_CLK, LOW);
  }
  digitalWrite(LED_SDI, LOW);
}

void latch595(void) {
  digitalWrite(LED_LAT, HIGH);
  digitalWrite(LED_LAT,  LOW);
}

uint8_t read_switch(void) {
  uint8_t res = 0;

  for(int8_t s = 0; s < 2; s++) digitalWrite(_scan[s], HIGH);

  for(int8_t s = 0; s < 2; s++) {
    digitalWrite(scan[s],  LOW);
    delayMicroseconds(20);

    for(int8_t r = 0; r < 4; r++) {
      res = (res << 1) + (digitalRead(_read[r]) == HIGH ? 0 : 1);
    }

    digitalWrite(_scan[s], HIGH);
  }

  return res;
}

void display(int8_t n) {
  static int8_t prev = -1;

  if(prev == n) return;
  prev = n;

#ifdef REV_1
  uint8_t vram[7] = {0};

  vram[0] = n;

  vram[6] = vram[0] / 100;
  vram[5] = (vram[0] / 10) % 10;
  vram[4] = vram[0] % 10;
  vram[3] = 0;
  vram[2] = vram[0] >> 4;
  vram[1] = vram[0] & 0x0F;

  vram[6] = font[vram[6]];
  vram[5] = font[vram[5]];
  vram[4] = font[vram[4]];
  vram[2] = font[vram[2]];
  vram[1] = font[vram[1]];

  if(vram[6] == font[0]) {
    vram[6] = 0;
    if(vram[5] == font[0]) {
      vram[5] = 0;
    }
  }

  if(vram[2] == font[0]) {
    vram[2] = 0;
  }
#endif

#ifdef REV_1_1
  uint8_t vram[6] = {0};

  vram[0] = n;

  vram[5] = vram[0] / 100;
  vram[4] = (vram[0] / 10) % 10;
  vram[3] = vram[0] % 10;
  vram[2] = vram[0] >> 4;
  vram[1] = vram[0] & 0x0F;

  vram[5] = font[vram[5]];
  vram[4] = font[vram[4]];
  vram[3] = font[vram[3]];
  vram[2] = font[vram[2]];
  vram[1] = font[vram[1]];

  if(vram[5] == font[0]) {
    vram[5] = 0;
    if(vram[4] == font[0]) {
      vram[4] = 0;
    }
  }

  if(vram[2] == font[0]) {
    vram[2] = 0;
  }
#endif

  for(int8_t i = 0; i < sizeof(vram); i++) {
    send595(vram[i]);
  }
  latch595();
}

void setup() {
  init_io();
  clear595();
}

void loop() {
  uint8_t n = read_switch();
  display(n);

  delay(25);
}
