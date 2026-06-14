/*
 * config.h
 * Aegis — Smart Safety Bracelet
 *
 * Central place for pin assignments, communication settings,
 * emergency contacts, and detection thresholds.
 *
 * Edit the values in this file to match your wiring and to set
 * your own emergency contact numbers before flashing.
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
//  I2C BUS — shared by MPU6050 (accelerometer/gyro) and
//  MAX30102 (heart-rate sensor)
// ============================================================
#define I2C_SDA_PIN         21
#define I2C_SCL_PIN         22

// ============================================================
//  GPS MODULE (NEO-6M) — UART2
//  GPS TX  -> ESP32 GPS_RX_PIN
//  GPS RX  -> ESP32 GPS_TX_PIN
// ============================================================
#define GPS_RX_PIN          16
#define GPS_TX_PIN          17
#define GPS_BAUD            9600

// ============================================================
//  GSM MODULE (SIM800L / A7670C) — UART1
//  GSM TX  -> ESP32 GSM_RX_PIN (direct)
//  GSM RXD <- ESP32 GSM_TX_PIN, THROUGH A VOLTAGE DIVIDER
//             (steps the ESP32 3.3V line down before reaching
//              the module's RXD pin — keep this divider in
//              your schematic for SIM800L / 5V-logic modules.
//              If your module is natively 3.3V logic, the
//              divider can be removed.)
// ============================================================
#define GSM_RX_PIN          26
#define GSM_TX_PIN          27
#define GSM_BAUD            9600

// ============================================================
//  MAX9814 MICROPHONE — analog output
// ============================================================
#define MIC_ANALOG_PIN      34   // ADC1_CH6 (input-only pin)

// ============================================================
//  SOS PUSH BUTTON — active LOW (button to GND, internal pull-up)
// ============================================================
#define SOS_BUTTON_PIN      4
#define SOS_HOLD_MS         800   // hold time to confirm SOS press

// ============================================================
//  LOCAL ALERT OUTPUTS
// ============================================================
#define STATUS_LED_PIN      2
#define BUZZER_PIN          25

// ============================================================
//  BATTERY VOLTAGE MONITOR (optional resistor divider to ADC)
// ============================================================
#define BATTERY_ADC_PIN     35
#define BATTERY_DIVIDER_RATIO 2.0f   // (R1+R2)/R2 — adjust to your divider

// ============================================================
//  EMERGENCY CONTACTS — include country code, e.g. "+91XXXXXXXXXX"
// ============================================================
#define EMERGENCY_NUMBER_1  "+91XXXXXXXXXX"
#define EMERGENCY_NUMBER_2  "+91XXXXXXXXXX"

// Optional: also place an automatic voice call to the primary
// contact when an alert triggers (1 = enabled, 0 = disabled)
#define ENABLE_EMERGENCY_CALL 0

// ============================================================
//  DETECTION THRESHOLDS — tune these on the bench before the demo
// ============================================================

// Fall / impact detection (MPU6050), values in multiples of g
#define FREEFALL_THRESHOLD_G    0.35f   // near-weightlessness = free fall
#define IMPACT_THRESHOLD_G      2.5f    // sudden spike = impact / shock

// Distress sound detection (MAX9814), raw ADC (0-4095)
#define SOUND_THRESHOLD         3000
#define SOUND_WINDOW_MS         50

// Heart-rate monitoring (MAX30102), beats per minute
#define HEART_RATE_LOW          40
#define HEART_RATE_HIGH         150
#define MIN_IR_FOR_FINGER       5000    // below this = no finger detected

// Minimum time between repeated alerts, to avoid SMS spam
#define ALERT_COOLDOWN_MS       30000UL // 30 seconds

#endif // CONFIG_H
