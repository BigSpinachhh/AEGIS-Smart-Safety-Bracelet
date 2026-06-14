/*
 * ============================================================
 *  RAKSHA — Smart Safety Bracelet
 *  ESP32 Firmware
 * ============================================================
 *
 * A wrist-worn safety device for women & child safety that
 * raises an SOS — over the cellular network, with live GPS
 * location — whenever any of the following happen:
 *
 *   1. The wearer presses and holds the SOS button
 *   2. A fall or sudden impact is detected (MPU6050)
 *   3. A loud distress sound is detected (MAX9814)
 *   4. Heart rate goes outside a safe range (MAX30102)
 *
 * On any trigger, the device:
 *   - Reads the latest GPS fix (NEO-6M)
 *   - Sends an SMS with a Google Maps link to emergency
 *     contacts via the GSM module (SIM800L / A7670C)
 *   - Sounds a local buzzer + LED alert
 *
 * --------------------------------------------------------
 *  REQUIRED LIBRARIES (install via Library Manager)
 * --------------------------------------------------------
 *   - TinyGPS++                              (Mikal Hart)
 *   - Adafruit MPU6050
 *   - Adafruit Unified Sensor
 *   - SparkFun MAX3010x Pulse and Proximity Sensor Library
 *
 * Board: ESP32 Dev Module
 * See ../../docs/SETUP.md for the full setup guide and
 * ../../docs/WIRING.md for the wiring table.
 * ============================================================
 */

#include <Wire.h>
#include <TinyGPS++.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <MAX30105.h>
#include "heartRate.h"
#include "config.h"

// ------------------------------------------------------------
//  Globals
// ------------------------------------------------------------
TinyGPSPlus   gps;
HardwareSerial gpsSerial(2);   // UART2 -> NEO-6M
HardwareSerial gsmSerial(1);   // UART1 -> SIM800L / A7670C

Adafruit_MPU6050 mpu;
MAX30105 heartSensor;

bool mpuReady   = false;
bool maxReady   = false;
bool gsmReady   = false;

// Heart-rate rolling average
const byte RATE_SIZE = 4;
byte  rateBuffer[RATE_SIZE];
byte  rateIndex   = 0;
long  lastBeatMs  = 0;
int   beatAvg     = 0;

// SOS button state
unsigned long sosPressStart = 0;
bool sosHeld = false;

// Free-fall flag for fall detection state machine
bool inFreeFall = false;

// Timers
unsigned long lastHeartCheckMs = 0;
unsigned long lastSoundCheckMs = 0;
unsigned long lastStatusMs     = 0;
unsigned long alertCooldownUntil = 0;

// ------------------------------------------------------------
//  Forward declarations
// ------------------------------------------------------------
void initMPU6050();
void initMAX30102();
void initGSM();
void feedGPS();
String getLocationLink();
void handleSOSButton();
void checkFallDetection();
void checkHeartRate();
void checkSoundLevel();
void triggerEmergency(const String &reason);
String sendATCommand(const String &command, unsigned long timeoutMs);
void sendSMS(const String &number, const String &message);
void soundAlarm();
void blinkStatus(int times, int delayMs);
void printStatus();
float readBatteryVoltage();

// ============================================================
//  SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println(F("\n========================================"));
  Serial.println(F("  RAKSHA Safety Bracelet — Booting"));
  Serial.println(F("========================================"));

  // I2C for MPU6050 + MAX30102
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  // SOS button + local alert outputs
  pinMode(SOS_BUTTON_PIN, INPUT_PULLUP);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // GPS (NEO-6M) on UART2
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println(F("[GPS] Serial started."));

  // GSM (SIM800L / A7670C) on UART1
  gsmSerial.begin(GSM_BAUD, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);

  initMPU6050();
  initMAX30102();
  initGSM();

  Serial.println(F("========================================"));
  Serial.println(F("  System Ready — monitoring for SOS"));
  Serial.println(F("========================================\n"));
  blinkStatus(3, 150);
}

// ============================================================
//  MAIN LOOP
// ============================================================
void loop() {
  feedGPS();
  handleSOSButton();

  if (mpuReady) {
    checkFallDetection();
  }

  if (maxReady && millis() - lastHeartCheckMs > 20) {
    lastHeartCheckMs = millis();
    checkHeartRate();
  }

  if (millis() - lastSoundCheckMs > SOUND_WINDOW_MS) {
    lastSoundCheckMs = millis();
    checkSoundLevel();
  }

  if (millis() - lastStatusMs > 5000) {
    lastStatusMs = millis();
    printStatus();
  }
}

// ============================================================
//  INITIALIZATION HELPERS
// ============================================================
void initMPU6050() {
  if (!mpu.begin()) {
    Serial.println(F("[MPU6050] NOT FOUND — fall detection disabled."));
    mpuReady = false;
    return;
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  mpuReady = true;
  Serial.println(F("[MPU6050] Initialized."));
}

void initMAX30102() {
  if (!heartSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println(F("[MAX30102] NOT FOUND — heart-rate monitoring disabled."));
    maxReady = false;
    return;
  }
  // Reasonable defaults for wrist-worn HR sensing
  byte ledBrightness = 0x3F; // ~12.6mA
  byte sampleAverage = 4;
  byte ledMode       = 2;    // Red + IR
  int  sampleRate    = 100;
  int  pulseWidth    = 411;
  int  adcRange      = 4096;

  heartSensor.setup(ledBrightness, sampleAverage, ledMode,
                     sampleRate, pulseWidth, adcRange);
  maxReady = true;
  Serial.println(F("[MAX30102] Initialized."));
}

void initGSM() {
  Serial.println(F("[GSM] Initializing module..."));
  sendATCommand("AT", 1000);          // wake / check
  sendATCommand("AT+CMGF=1", 1000);   // SMS text mode
  sendATCommand("AT+CNMI=1,2,0,0,0", 1000); // route incoming SMS to UART (optional)
  gsmReady = true;
  Serial.println(F("[GSM] Ready."));
}

// ============================================================
//  GPS
// ============================================================
void feedGPS() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
}

String getLocationLink() {
  if (gps.location.isValid()) {
    double lat = gps.location.lat();
    double lng = gps.location.lng();
    String link = "https://maps.google.com/?q=";
    link += String(lat, 6);
    link += ",";
    link += String(lng, 6);
    return link;
  }
  return "Location unavailable (no GPS fix yet)";
}

// ============================================================
//  SOS BUTTON — press and hold to trigger
// ============================================================
void handleSOSButton() {
  bool pressed = (digitalRead(SOS_BUTTON_PIN) == LOW);

  if (pressed && !sosHeld) {
    sosHeld = true;
    sosPressStart = millis();
  } else if (!pressed && sosHeld) {
    sosHeld = false; // released before hold time — ignore
  }

  if (sosHeld && (millis() - sosPressStart >= SOS_HOLD_MS)) {
    triggerEmergency("Manual SOS button pressed");
    sosHeld = false; // require release + re-press for next trigger
  }
}

// ============================================================
//  FALL / IMPACT DETECTION (MPU6050)
// ============================================================
void checkFallDetection() {
  sensors_event_t accel, gyro, temp;
  if (!mpu.getEvent(&accel, &gyro, &temp)) return;

  // Magnitude of acceleration vector, in g (9.81 m/s^2 = 1g)
  float totalAccelG = sqrt(sq(accel.acceleration.x) +
                            sq(accel.acceleration.y) +
                            sq(accel.acceleration.z)) / 9.81f;

  if (totalAccelG < FREEFALL_THRESHOLD_G) {
    // Near-zero G — wearer's wrist is in free fall
    inFreeFall = true;
  } else if (inFreeFall && totalAccelG > IMPACT_THRESHOLD_G) {
    // Free fall followed by a sharp impact = a fall to the ground
    inFreeFall = false;
    triggerEmergency("Fall detected (free-fall + impact)");
  } else if (totalAccelG > IMPACT_THRESHOLD_G) {
    // Sudden hard impact without a preceding free fall
    // (e.g. being struck, grabbed, or a hard knock)
    inFreeFall = false;
    triggerEmergency("Sudden impact / shock detected");
  } else {
    inFreeFall = false;
  }
}

// ============================================================
//  HEART RATE MONITORING (MAX30102)
// ============================================================
void checkHeartRate() {
  long irValue = heartSensor.getIR();

  if (irValue < MIN_IR_FOR_FINGER) {
    return; // no contact with skin — skip
  }

  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeatMs;
    lastBeatMs = millis();

    float bpm = 60.0f / (delta / 1000.0f);

    if (bpm > 20 && bpm < 255) {
      rateBuffer[rateIndex++] = (byte)bpm;
      rateIndex %= RATE_SIZE;

      int total = 0;
      for (byte i = 0; i < RATE_SIZE; i++) total += rateBuffer[i];
      beatAvg = total / RATE_SIZE;

      if (beatAvg > 0 &&
          (beatAvg < HEART_RATE_LOW || beatAvg > HEART_RATE_HIGH)) {
        triggerEmergency("Abnormal heart rate detected (" +
                          String(beatAvg) + " bpm)");
      }
    }
  }
}

// ============================================================
//  DISTRESS SOUND DETECTION (MAX9814)
// ============================================================
void checkSoundLevel() {
  static int peakValue = 0;
  static unsigned long sampleStart = 0;

  int sample = analogRead(MIC_ANALOG_PIN);
  if (sample > peakValue) peakValue = sample;

  if (millis() - sampleStart >= SOUND_WINDOW_MS) {
    if (peakValue > SOUND_THRESHOLD) {
      triggerEmergency("Loud distress sound detected");
    }
    peakValue = 0;
    sampleStart = millis();
  }
}

// ============================================================
//  EMERGENCY TRIGGER
// ============================================================
void triggerEmergency(const String &reason) {
  if (millis() < alertCooldownUntil) {
    Serial.print(F("[ALERT] Suppressed (cooldown active): "));
    Serial.println(reason);
    return;
  }

  Serial.print(F("[ALERT] TRIGGERED: "));
  Serial.println(reason);

  String message = "EMERGENCY ALERT!\n";
  message += reason;
  message += "\nLocation: ";
  message += getLocationLink();

  sendSMS(EMERGENCY_NUMBER_1, message);
  sendSMS(EMERGENCY_NUMBER_2, message);

#if ENABLE_EMERGENCY_CALL
  sendATCommand(String("ATD") + EMERGENCY_NUMBER_1 + ";", 5000);
#endif

  soundAlarm();

  alertCooldownUntil = millis() + ALERT_COOLDOWN_MS;
}

// ============================================================
//  GSM / SMS HELPERS (AT commands)
// ============================================================
String sendATCommand(const String &command, unsigned long timeoutMs) {
  // Clear any stale data
  while (gsmSerial.available()) gsmSerial.read();

  gsmSerial.println(command);

  String response = "";
  unsigned long start = millis();
  while (millis() - start < timeoutMs) {
    while (gsmSerial.available()) {
      response += (char)gsmSerial.read();
    }
  }

  Serial.print(F("[GSM] > ")); Serial.println(command);
  if (response.length() > 0) {
    Serial.print(F("[GSM] < ")); Serial.println(response);
  }
  return response;
}

void sendSMS(const String &number, const String &message) {
  sendATCommand("AT+CMGF=1", 300);                 // ensure text mode

  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(number);
  gsmSerial.println("\"");
  delay(300);

  gsmSerial.print(message);
  delay(100);
  gsmSerial.write(26); // Ctrl+Z — sends the SMS

  // Allow time for the network to accept the message
  unsigned long start = millis();
  while (millis() - start < 3000) {
    while (gsmSerial.available()) Serial.write(gsmSerial.read());
  }

  Serial.print(F("[GSM] SMS dispatched to "));
  Serial.println(number);
}

// ============================================================
//  LOCAL ALERTS
// ============================================================
void soundAlarm() {
  for (int i = 0; i < 6; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(150);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(150);
  }
}

void blinkStatus(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(delayMs);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(delayMs);
  }
}

// ============================================================
//  BATTERY MONITOR (optional)
// ============================================================
float readBatteryVoltage() {
  int raw = analogRead(BATTERY_ADC_PIN);
  float vAtPin = (raw / 4095.0f) * 3.3f;
  return vAtPin * BATTERY_DIVIDER_RATIO;
}

// ============================================================
//  STATUS PRINT (debug, every 5s)
// ============================================================
void printStatus() {
  Serial.println(F("---------------- STATUS ----------------"));

  Serial.print(F("GPS Fix : "));
  if (gps.location.isValid()) {
    Serial.print(F("YES  ("));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(", "));
    Serial.print(gps.location.lng(), 6);
    Serial.println(F(")"));
  } else {
    Serial.println(F("NO"));
  }

  Serial.print(F("Heart Rate (avg): "));
  Serial.print(beatAvg);
  Serial.println(F(" bpm"));

  Serial.print(F("Battery   : "));
  Serial.print(readBatteryVoltage(), 2);
  Serial.println(F(" V"));

  Serial.println(F("------------------------------------------\n"));
}
