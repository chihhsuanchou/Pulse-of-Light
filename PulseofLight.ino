#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define LED_PIN     6
#define LED_COUNT   8  // 根據你實際LED數量調整
#define TRIG_PIN    9
#define ECHO_PIN    10
#define SERVO_PIN   5

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Servo myServo;

int waveColorIndex = 0;
uint32_t waveColors[] = {
  strip.Color(0, 150, 255),   // 藍
  strip.Color(255, 0, 0),     // 紅
  strip.Color(0, 255, 0),     // 綠
  strip.Color(255, 255, 0),   // 黃
  strip.Color(255, 0, 255),   // 紫
  strip.Color(0, 255, 255),   // 青
  strip.Color(255, 150, 0)    // 橘
};

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  strip.begin();
  strip.show(); // 全部熄滅
  myServo.attach(SERVO_PIN);
  randomSeed(analogRead(0));
}

void loop() {
  float distance = readDistance();
  Serial.print("距離: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < 10) {
    waveGlowColorCycle();
    slowServoMotion();
  } else if (distance >= 10 && distance <= 30) {
    fastRainbowWithFlicker();
    fastServoMotion();
  } else {
    randomDualFade();
    myServo.write(90); // 停在中間
  }
}

float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void fastRainbowWithFlicker() {
  static uint16_t j = 0;
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    uint8_t flicker = random(100, 256);
    uint32_t color = Wheel((i * 256 / strip.numPixels() + j) & 255);
    color = scaleColor(color, flicker);
    strip.setPixelColor(i, color);
  }
  strip.show();
  j = (j + 10) & 0xFFFF;
  delay(20);
}

void waveGlowColorCycle() {
  static int waveCount = 0;
  waveCount++;
  if (waveCount % 3 == 0) {
    waveColorIndex = (waveColorIndex + 1) % (sizeof(waveColors) / sizeof(waveColors[0]));
  }
  uint32_t currentColor = waveColors[waveColorIndex];

  for (int i = 0; i < LED_COUNT / 2; i++) {
    int opposite = LED_COUNT - 1 - i;
    strip.setPixelColor(i, currentColor);
    strip.setPixelColor(opposite, currentColor);
    strip.show();
    delay(50);
    strip.setPixelColor(i, 0);
    strip.setPixelColor(opposite, 0);
  }
  strip.show();
  delay(80);
}

void randomDualFade() {
  uint32_t color1 = randomColor();
  uint32_t color2 = randomColor();
  for (int b = 0; b < 256; b += 5) {
    fadeTwoColors(color1, color2, b);
    delay(10);
  }
  for (int b = 255; b >= 0; b -= 5) {
    fadeTwoColors(color1, color2, b);
    delay(10);
  }
}

void fadeTwoColors(uint32_t c1, uint32_t c2, uint8_t brightness) {
  for (int i = 0; i < LED_COUNT; i++) {
    uint32_t c = (i % 2 == 0) ? c1 : c2;
    strip.setPixelColor(i, scaleColor(c, brightness));
  }
  strip.show();
}

void slowServoMotion() {
  static bool toggle = false;
  myServo.write(toggle ? 80 : 100);
  toggle = !toggle;
  delay(500);
}

void fastServoMotion() {
  static bool toggle = false;
  myServo.write(toggle ? 60 : 120);
  toggle = !toggle;
  delay(150);
}

uint32_t randomColor() {
  return strip.Color(random(0, 256), random(0, 256), random(0, 256));
}

uint32_t scaleColor(uint32_t color, uint8_t brightness) {
  uint8_t r = (uint8_t)((color >> 16) & 0xFF);
  uint8_t g = (uint8_t)((color >> 8) & 0xFF);
  uint8_t b = (uint8_t)(color & 0xFF);

  r = r * brightness / 255;
  g = g * brightness / 255;
  b = b * brightness / 255;

  return strip.Color(r, g, b);
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

void setAll(uint32_t c) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}










