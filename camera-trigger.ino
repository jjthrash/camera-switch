#define LED1 5
#define LED2 6
#define LED3 7
#define LED4 8
#define LED5 9
#define LED6 10

#define ENCODER1 2
#define ENCODER2 3

#define SHOOT 4

#define SHUTTER 1

// important values
unsigned char parameter = 0;
typedef enum {
  Start, Config, Shoot
} Mode;
Mode mode = Start;

// utility values
unsigned int encoder1Last = 0;
unsigned int encoder2Last = 0;
unsigned int encoderInterruptMicros = 0;
#define DEBOUNCE_THRESHOLD_MICROS 100

void setup() {
//  Serial.begin(9600);
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);

  pinMode(SHOOT, INPUT);
  pinMode(SHUTTER, OUTPUT);

  pinMode(ENCODER1, INPUT_PULLUP);
  pinMode(ENCODER2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER1), encoder1Changed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER2), encoder2Changed, CHANGE);

  encoder1Last = digitalRead(ENCODER1);
  encoder2Last = digitalRead(ENCODER2);
}

void loop() {
  switch (mode) {
    case Start:
    doStart();
    break;
    
    case Config:
    doConfig();
    break;

    case Shoot:
    digitalWrite(SHUTTER, HIGH);
    delay(100);
    digitalWrite(SHUTTER, LOW);
    displayShoot();
    mode = Config;
    break;
  }
}

char startSequence[] = { 1, 2, 4, 8, 16, 32, 16, 8, 4, 2 };

void doStart() {
  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < sizeof(startSequence); i++) {
       displayValue(startSequence[i]);
       delay(100);
    }
  }

  mode = Config;
}

void doConfig() {
  displayValue(parameter);
  if (digitalRead(SHOOT) == HIGH) {
    mode = Shoot;
  }
}

void displayValue(char value) {
  digitalWrite(LED1, (value & 1) > 0);
  digitalWrite(LED2, (value & (1 << 1)) > 0);
  digitalWrite(LED3, (value & (1 << 2)) > 0);
  digitalWrite(LED4, (value & (1 << 3)) > 0);
  digitalWrite(LED5, (value & (1 << 4)) > 0);
  digitalWrite(LED6, (value & (1 << 5)) > 0);
//  Serial.print(parameter);
//  Serial.println("");
//  Serial.print(encoder1Last);
//  Serial.write(", ");
//  Serial.print(encoder2Last);
//  Serial.println("");
}

char shootOn = 0;

void displayShoot() {
  displayValue(shootOn ? 63 : 0);
  shootOn = 1 - shootOn;
  delay(500);
}

/**
 * CW:
 * 11
 * 10
 * 00
 * 01
 */
void encoder1Changed() {
  if (micros() - encoderInterruptMicros < DEBOUNCE_THRESHOLD_MICROS)
    return;
    
  encoderInterruptMicros = micros();
  encoder1Last ^ encoder2Last ? incrementParameter() : decrementParameter();
  encoder1Last = digitalRead(ENCODER1);
  displayValue(parameter);
}

void incrementParameter() {
  parameter = parameter + 1;
  parameter %= 64;
}

void decrementParameter() {
  parameter = parameter == 0 ? 63 : parameter - 1;
}

void encoder2Changed() {
  if (micros() - encoderInterruptMicros < DEBOUNCE_THRESHOLD_MICROS)
    return;

  encoderInterruptMicros = micros();
  encoder2Last = digitalRead(ENCODER2);
  encoder1Last ^ encoder2Last ? incrementParameter() : decrementParameter();
  displayValue(parameter);
}

