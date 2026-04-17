#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// =====================
// YOUR DETAILS
// =====================
const char* ssid     = "xxxxxx";
const char* password = "xxxxxxxxxx";
String BOTtoken      = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
String CHAT_ID       = "xxxxxxxxxxx";

// =====================
// PIN DEFINITIONS
// =====================
#define THUMB_PIN   34
#define INDEX_PIN   32
#define MIDDLE_PIN  35
#define RING_PIN    33
#define PINKY_PIN   25

// =====================
// THRESHOLDS
// =====================
#define THUMB_THRESHOLD   700
#define INDEX_THRESHOLD   700
#define MIDDLE_THRESHOLD  700
#define RING_THRESHOLD    700
#define PINKY_THRESHOLD   700

// =====================
// OBJECTS
// =====================
LiquidCrystal_I2C lcd(0x27, 16, 2);
HardwareSerial mySerial(2);
DFRobotDFPlayerMini myDFPlayer;
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// =====================
// GESTURE TRACKING
// =====================
unsigned long lastTriggerTime = 0;
#define GESTURE_DELAY 3000

// =====================
// SETUP
// =====================
void setup() {
  Serial.begin(115200);

  // LCD
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sign Language");
  lcd.setCursor(0, 1);
  lcd.print("Glove Ready!");
  delay(2000);

  // DFPlayer
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  delay(1000);
  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer Error!");
    lcd.clear();
    lcd.print("DFPlayer Error!");
    delay(2000);
  } else {
    myDFPlayer.volume(25);
    Serial.println("DFPlayer OK!");
  }

  // WiFi
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected!");
    lcd.clear();
    lcd.print("WiFi Connected!");
    delay(2000);
  } else {
    Serial.println("WiFi Failed!");
    lcd.clear();
    lcd.print("WiFi Failed!");
    delay(2000);
  }

  client.setInsecure();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready!");
  lcd.setCursor(0, 1);
  lcd.print("Wear Glove");
}

// =====================
// TRIGGER OUTPUT
// =====================
void triggerGesture(String message, int soundFile) {
  if (millis() - lastTriggerTime < GESTURE_DELAY) return;
  lastTriggerTime = millis();

  Serial.println("Gesture: " + message);

  // LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Detected:");
  lcd.setCursor(0, 1);
  lcd.print(message);

  // Speaker
  myDFPlayer.play(soundFile);

  // Telegram
  if (WiFi.status() == WL_CONNECTED) {
    bot.sendMessage(CHAT_ID, "🤟 " + message, "");
  }
}

// =====================
// DETECT GESTURE
// =====================
void detectGesture() {
  bool thumb  = analogRead(THUMB_PIN)  < THUMB_THRESHOLD;
  bool index  = analogRead(INDEX_PIN)  < INDEX_THRESHOLD;
  bool middle = analogRead(MIDDLE_PIN) < MIDDLE_THRESHOLD;
  bool ring   = analogRead(RING_PIN)   < RING_THRESHOLD;
  bool pinky  = analogRead(PINKY_PIN)  < PINKY_THRESHOLD;

  // Debug
  Serial.print("T:"); Serial.print(analogRead(THUMB_PIN));
  Serial.print(" I:"); Serial.print(analogRead(INDEX_PIN));
  Serial.print(" M:"); Serial.print(analogRead(MIDDLE_PIN));
  Serial.print(" R:"); Serial.print(analogRead(RING_PIN));
  Serial.print(" P:"); Serial.println(analogRead(PINKY_PIN));

  // =====================
  // ALL 5 BENT
  // =====================
  if (thumb && index && middle && ring && pinky) {
    triggerGesture("SOS Save Me", 21);
  }

  // =====================
  // 3 FINGER COMBINATIONS
  // =====================
  else if (thumb && index && middle && !ring && !pinky) {
    triggerGesture("Take Me Hospital", 16);
  }
  else if (thumb && index && !middle && ring && !pinky) {
    triggerGesture("I Am Lost", 17);
  }
  else if (thumb && index && !middle && !ring && pinky) {
    triggerGesture("Please Help Me", 18);
  }
  else if (!thumb && index && middle && ring && !pinky) {
    triggerGesture("I Need Assistance", 19);
  }
  else if (!thumb && index && middle && !ring && pinky) {
    triggerGesture("Emergency", 20);
  }

  // =====================
  // 2 FINGER COMBINATIONS
  // =====================
  else if (thumb && index && !middle && !ring && !pinky) {
    triggerGesture("I Am In Pain", 6);
  }
  else if (thumb && !index && middle && !ring && !pinky) {
    triggerGesture("Call Doctor", 7);
  }
  else if (thumb && !index && !middle && ring && !pinky) {
    triggerGesture("Call Police", 8);
  }
  else if (thumb && !index && !middle && !ring && pinky) {
    triggerGesture("Call My Family", 9);
  }
  else if (!thumb && index && middle && !ring && !pinky) {
    triggerGesture("I Need Water", 10);
  }
  else if (!thumb && index && !middle && ring && !pinky) {
    triggerGesture("I Need Food", 11);
  }
  else if (!thumb && index && !middle && !ring && pinky) {
    triggerGesture("I Need Medicine", 12);
  }
  else if (!thumb && !index && middle && ring && !pinky) {
    triggerGesture("I Cannot Speak", 13);
  }
  else if (!thumb && !index && middle && !ring && pinky) {
    triggerGesture("Please Wait", 14);
  }
  else if (!thumb && !index && !middle && ring && pinky) {
    triggerGesture("Thank You", 15);
  }

  // =====================
  // SINGLE FINGER
  // =====================
  else if (thumb && !index && !middle && !ring && !pinky) {
    triggerGesture("Hello", 1);
  }
  else if (!thumb && index && !middle && !ring && !pinky) {
    triggerGesture("Help Me", 2);
  }
  else if (!thumb && !index && middle && !ring && !pinky) {
    triggerGesture("Yes", 3);
  }
  else if (!thumb && !index && !middle && ring && !pinky) {
    triggerGesture("No", 4);
  }
  else if (!thumb && !index && !middle && !ring && pinky) {
    triggerGesture("Goodbye", 5);
  }
}

// =====================
// LOOP
// =====================
void loop() {
  detectGesture();
  delay(2000);
}