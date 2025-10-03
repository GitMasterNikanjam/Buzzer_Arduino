/**
 * @file Buzzer_Test.ino
 * @brief Comprehensive test for the Buzzer library (finite/infinite blink + blocking/non-blocking presets).
 *
 * Library API used here:
 *  - bool init(); BuzzerError lastError(); static const char* errorToCstr(BuzzerError)
 *  - on(), off(), isOn()
 *  - startBlink(on_ms, off_ms, repeat), stopBlink()
 *  - playPreset(BuzzerPreset preset, bool non_blocking, uint32_t repeat), stopPreset()
 *  - stopAll(), isBusy(), update()
 *
 * Notes:
 *  - repeat == 0 => infinite (non-blocking modes)
 *  - Call update() frequently in loop() for non-blocking operations
 */

#include <Arduino.h>
#include "Buzzer.h"

// -------------------- User configuration --------------------
static const int BUZZER_PIN = 7;   // change to your pin
static const uint8_t ACTIVE_MODE = 1; // 1 = active-HIGH, 0 = active-LOW

Buzzer bz;

// -------------------- Menu helpers --------------------
void printMenu() {
  Serial.println(F("\n===== Buzzer Test Menu ====="));
  Serial.println(F("1) Direct ON"));
  Serial.println(F("2) Direct OFF"));
  Serial.println(F("3) Blink finite (100ms ON / 200ms OFF, 5 pulses)"));
  Serial.println(F("4) Blink infinite (100ms ON / 900ms OFF) - non-blocking"));
  Serial.println(F("5) Stop blink"));
  Serial.println(F("6) Preset: Init - blocking x1"));
  Serial.println(F("7) Preset: Stop - blocking x1"));
  Serial.println(F("8) Preset: Warning1 - non-blocking, repeat=1"));
  Serial.println(F("9) Preset: Warning2 - non-blocking, repeat=0 (infinite)"));
  Serial.println(F("a) Preset: Warning3 - blocking x1"));
  Serial.println(F("b) Preset: Heartbeat - non-blocking, infinite"));
  Serial.println(F("c) Preset: ErrorAlert - non-blocking, repeat=2"));
  Serial.println(F("d) Preset: Success - blocking x1"));
  Serial.println(F("e) Preset: Alarm - non-blocking, repeat=0 (infinite)"));
  Serial.println(F("f) Preset: MorseSOS - non-blocking, repeat=1"));
  Serial.println(F("s) Stop preset"));
  Serial.println(F("x) stopAll()"));
  Serial.println(F("i) Info (initFlag, busy, isOn, lastError)"));
  Serial.println(F("============================\n"));
}

void printInfo() {
  Serial.print(F("initFlag=")); Serial.print(bz.getInitFlag());
  Serial.print(F(", busy=")); Serial.print(bz.isBusy());
  Serial.print(F(", isOn=")); Serial.print(bz.isOn());
  Serial.print(F(", lastError=")); Serial.println(Buzzer::errorToCstr(bz.lastError()));
}

// -------------------- Setup --------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) { /* wait for USB serial (if needed) */ }

  Serial.println(F("\n[Buzzer Test]"));

  // Configure parameters then init
  bz.parameters.PIN_NUM = BUZZER_PIN;
  bz.parameters.ACTIVE_MODE = ACTIVE_MODE; // 1: active-HIGH, 0: active-LOW

  if (!bz.init()) { // must check return!
    Serial.print(F("Init failed: "));
    Serial.println(Buzzer::errorToCstr(bz.lastError())); // no String allocation
    // Don't proceed further if init failed
    while (1) { delay(1000); }
  }

  Serial.println(F("Init OK."));
  printMenu();
}

// -------------------- Loop: poll menu + run non-blocking engines --------------------
void loop() {
  // Keep scheduler running for non-blocking modes:
  bz.update();

  // Handle user input
  if (Serial.available()) {
    char ch = Serial.read();

    switch (ch) {
      case '1': // Direct ON
        bz.on();
        Serial.println(F("ON"));
        break;

      case '2': // Direct OFF
        bz.off();
        Serial.println(F("OFF"));
        break;

      case '3': // Blink finite
        bz.startBlink(100, 200, 5);
        Serial.println(F("Blink finite: 5 pulses (100ms ON / 200ms OFF)"));
        break;

      case '4': // Blink infinite
        bz.startBlink(100, 900, 0);
        Serial.println(F("Blink infinite: 100/900 ms (non-blocking)"));
        break;

      case '5': // Stop blink
        bz.stopBlink();
        Serial.println(F("stopBlink() called"));
        break;

      case '6': // Preset Init blocking
        bz.playPreset(BuzzerPreset::Init, /*non_blocking=*/false, /*repeat=*/1);
        Serial.println(F("Preset Init: blocking x1 (done)"));
        break;

      case '7': // Preset Stop blocking
        bz.playPreset(BuzzerPreset::Stop, false, 1);
        Serial.println(F("Preset Stop: blocking x1 (done)"));
        break;

      case '8': // Preset Warning1 non-blocking repeat=1
        bz.playPreset(BuzzerPreset::Warning1, true, 1);
        Serial.println(F("Preset Warning1: non-blocking, repeat=1"));
        break;

      case '9': // Preset Warning2 non-blocking infinite
        bz.playPreset(BuzzerPreset::Warning2, true, 0);
        Serial.println(F("Preset Warning2: non-blocking, infinite"));
        break;

      case 'a': // Preset Warning3 blocking
        bz.playPreset(BuzzerPreset::Warning3, false, 1);
        Serial.println(F("Preset Warning3: blocking x1 (done)"));
        break;

      case 'b': // Heartbeat non-blocking infinite
        bz.playPreset(BuzzerPreset::Heartbeat, true, 0);
        Serial.println(F("Preset Heartbeat: non-blocking, infinite"));
        break;

      case 'c': // ErrorAlert non-blocking repeat=2
        bz.playPreset(BuzzerPreset::ErrorAlert, true, 2);
        Serial.println(F("Preset ErrorAlert: non-blocking, repeat=2"));
        break;

      case 'd': // Success blocking x1
        bz.playPreset(BuzzerPreset::Success, false, 1);
        Serial.println(F("Preset Success: blocking x1 (done)"));
        break;

      case 'e': // Alarm non-blocking infinite
        bz.playPreset(BuzzerPreset::Alarm, true, 0);
        Serial.println(F("Preset Alarm: non-blocking, infinite"));
        break;

      case 'f': // Morse SOS non-blocking once
        bz.playPreset(BuzzerPreset::MorseSOS, true, 1);
        Serial.println(F("Preset MorseSOS: non-blocking, repeat=1"));
        break;

      case 's': // Stop preset
        bz.stopPreset();
        Serial.println(F("stopPreset() called"));
        break;

      case 'x': // Stop everything
        bz.stopAll();
        Serial.println(F("stopAll() called"));
        break;

      case 'i':
        printInfo();
        break;

      case '\n':
      case '\r':
        break;

      default:
        printMenu();
        break;
    }
  }
}
