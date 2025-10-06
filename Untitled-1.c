/* Robot biểu cảm - Arduino
   Phần cứng: servo mắt (servoEye), servo miệng (servoMouth), NeoPixel cho "mắt"
   Thư viện: Servo, Adafruit_NeoPixel
*/
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

#define PIN_NEOPIXEL 6   // pin NeoPixel
#define NUMPIXELS 2      // hai "mắt"
#define SERVO_EYE_PIN 9
#define SERVO_MOUTH_PIN 10
#define BUTTON_PIN 2     // (tuỳ ý) bấm để đổi cảm xúc

Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Servo servoEye;
Servo servoMouth;

unsigned long lastBlink = 0;
int blinkInterval = 3500; // ms
bool blinking = false;
int blinkStage = 0;

enum Emotion { NEUTRAL, HAPPY, SAD, ANGRY, SURPRISED };
Emotion emotion = NEUTRAL;

void setup() {
  Serial.begin(9600);
  pixels.begin();
  servoEye.attach(SERVO_EYE_PIN);
  servoMouth.attach(SERVO_MOUTH_PIN);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  setEmotion(NEUTRAL);
  lastBlink = millis();
}

void loop() {
  // button to cycle emotion
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(150);
    cycleEmotion();
    while(digitalRead(BUTTON_PIN)==LOW) delay(10);
  }

  // auto blink occasionally
  if (millis() - lastBlink > blinkInterval) {
    doBlink();
    lastBlink = millis();
    blinkInterval = random(2000, 6000);
  }

  // small idle animations per emotion
  animateEmotion();
}

// change emotion and update servos/LEDs
void setEmotion(Emotion e) {
  emotion = e;
  switch(e) {
    case NEUTRAL:
      setEyesColor(0, 150, 200);
      servoEye.write(5);   // slightly open
      servoMouth.write(10); // neutral mouth
      break;
    case HAPPY:
      setEyesColor(0, 255, 100);
      servoEye.write(5);
      servoMouth.write(40); // smile
      break;
    case SAD:
      setEyesColor(0, 0, 180);
      servoEye.write(10); // a bit droopy
      servoMouth.write(0); // frown (depending on mech)
      break;
    case ANGRY:
      setEyesColor(255, 40, 0);
      servoEye.write(0);   // narrow
      servoMouth.write(80); // tense
      break;
    case SURPRISED:
      setEyesColor(255, 255, 150);
      servoEye.write(0);   // wide open
      servoMouth.write(70); // big O
      break;
  }
}

// small continuous animation per emotion
void animateEmotion() {
  static unsigned long t0 = 0;
  unsigned long t = millis();
  if (emotion == HAPPY) {
    // gentle mouth bob
    int m = 30 + (sin(t/200.0) * 8);
    servoMouth.write(m);
  } else if (emotion == SAD) {
    // slow droop-eyes sway
    int epos = 8 + (sin(t/800.0) * 3);
    servoEye.write(epos);
  } else if (emotion == NEUTRAL) {
    int m = 10 + (sin(t/1000.0) * 3);
    servoMouth.write(m);
  } else if (emotion == ANGRY) {
    // subtle eye twitch
    if ((t/300) % 10 == 0) servoEye.write(random(0,6));
  } else if (emotion == SURPRISED) {
    // keep wide open
  }
}

// simple blink sequence
void doBlink() {
  // close
  for (int p=5; p<=90; p+=12) {
    servoEye.write(p);
    delay(20);
  }
  delay(80);
  // open
  for (int p=90; p>=5; p-=12) {
    servoEye.write(p);