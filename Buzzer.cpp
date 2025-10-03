#include "Buzzer.h"

// ------------------------------ Error strings ------------------------------
const char* Buzzer::errorToCstr(BuzzerError e) {
  switch (e) {
    case BuzzerError::None:              return "No error";
    case BuzzerError::InvalidPin:        return "Invalid pin";
    case BuzzerError::InvalidActiveMode: return "Invalid ACTIVE_MODE (must be 0 or 1)";
    case BuzzerError::NotInitialized:    return "Buzzer not initialized";
    default:                             return "Unknown error";
  }
}

// ------------------------------ Lifetime ------------------------------
Buzzer::~Buzzer() {
  if (_initFlag && parameters.PIN_NUM >= 0) {
    pinMode(parameters.PIN_NUM, INPUT);
  }
}

// ------------------------------ Initialization ------------------------------
bool Buzzer::init() {
  _initFlag  = false;
  _lastError = BuzzerError::None;

  if (!_checkParameters()) return false;

  pinMode(parameters.PIN_NUM, OUTPUT);
  digitalWrite(parameters.PIN_NUM, _offLevel());
  _currentOn = false;

  // reset engines
  _mode = Mode::Idle;
  _blinkOnPhase = false;
  _blinkRemain = 0;
  _presetRepeatRemain = 0;
  _step = _sub = _burst = 0;

  _initFlag = true;
  return true;
}

bool Buzzer::_checkParameters() {
  if (parameters.PIN_NUM < 0) {
    _lastError = BuzzerError::InvalidPin;
    return false;
  }
  if (parameters.ACTIVE_MODE > 1) {
    _lastError = BuzzerError::InvalidActiveMode;
    return false;
  }
  _lastError = BuzzerError::None;
  return true;
}

// ------------------------------ Direct control ------------------------------
void Buzzer::on() {
  if (!_initFlag) { _lastError = BuzzerError::NotInitialized; return; }
  digitalWrite(parameters.PIN_NUM, _onLevel());
  _currentOn = true;
}

void Buzzer::off() {
  if (!_initFlag) { _lastError = BuzzerError::NotInitialized; return; }
  digitalWrite(parameters.PIN_NUM, _offLevel());
  _currentOn = false;
}

// ------------------------------ Blocking presets (one pass per call) ------------------------------
void Buzzer::soundInit()      { on();  delay(1000); off(); delay(100); }
void Buzzer::soundStop()      { for (uint8_t i=0;i<2;++i){ on(); delay(100); off(); delay(100);} }
void Buzzer::soundWarning_1() {
  for (uint8_t burst = 0; burst < 10; ++burst) {
    for (uint8_t i = 0; i < 20; ++i) { on(); delay(5); off(); delay(5); }
    delay(200);
  }
}
void Buzzer::soundWarning_2() { on();  delay(100); off(); delay(100); }
void Buzzer::soundWarning_3() { for (uint8_t i=0;i<10;++i){ on(); delay(25); off(); delay(25);} }

// New blocking helpers (single sequence)
void Buzzer::soundHeartbeat()   { on(); delay(50);  off(); delay(950); }
void Buzzer::soundErrorAlert()  { for(uint8_t i=0;i<3;++i){ on(); delay(200); off(); delay(100);} delay(1000); }
void Buzzer::soundSuccess()     { on(); delay(100); off(); delay(50); on(); delay(400); off(); }
void Buzzer::soundAlarm()       { on(); delay(2000); off(); delay(200); }
void Buzzer::soundMorseSOS() {
  // Morse timing unit = 100ms
  auto dot  = [this](){ on(); delay(100); off(); delay(100); };        // 1 unit on + 1 gap
  auto dash = [this](){ on(); delay(300); off(); delay(100); };        // 3 unit on + 1 gap

  // S (· · ·)
  dot(); dot(); dot();
  delay(200); // between letters (~3 units total; we already had 1, add 2)

  // O (— — —)
  dash(); dash(); dash();
  delay(200); // between letters

  // S (· · ·)
  dot(); dot(); dot();
  // (no extra word gap; one sequence only)
}

// ------------------------------ Blink (non-blocking) ------------------------------
void Buzzer::startBlink(uint16_t on_ms, uint16_t off_ms, uint32_t repeat) {
  if (!_initFlag) { _lastError = BuzzerError::NotInitialized; return; }
  _mode          = Mode::Blink;
  _blinkOnMs     = on_ms;
  _blinkOffMs    = off_ms;
  _blinkRemain   = repeat;                // 0 ⇒ infinite
  _blinkOnPhase  = true;                  // start with ON
  on();
  _nextToggleMs  = millis() + _blinkOnMs;
}

void Buzzer::stopBlink() {
  if (!_initFlag) { _lastError = BuzzerError::NotInitialized; return; }
  if (_mode == Mode::Blink) {
    _mode = Mode::Idle;
    off();
  }
}

// ------------------------------ Preset (blocking or non-blocking) ------------------------------
void Buzzer::playPreset(BuzzerPreset preset, bool non_blocking, uint32_t repeat) {
  if (!_initFlag) { _lastError = BuzzerError::NotInitialized; return; }

  if (!non_blocking) {
    // Blocking path: execute preset repeat times (repeat must be >=1)
    if (repeat == 0) repeat = 1;
    for (uint32_t r = 0; r < repeat; ++r) {
      switch (preset) {
        case BuzzerPreset::Init:       soundInit();      break;
        case BuzzerPreset::Stop:       soundStop();      break;
        case BuzzerPreset::Warning1:   soundWarning_1(); break;
        case BuzzerPreset::Warning2:   soundWarning_2(); break;
        case BuzzerPreset::Warning3:   soundWarning_3(); break;
        case BuzzerPreset::Heartbeat:  soundHeartbeat(); break;
        case BuzzerPreset::ErrorAlert: soundErrorAlert();break;
        case BuzzerPreset::Success:    soundSuccess();   break;
        case BuzzerPreset::Alarm:      soundAlarm();     break;
        case BuzzerPreset::MorseSOS:   soundMorseSOS();  break;
      }
    }
    return;
  }

  // Non-blocking path
  _mode = Mode::Preset;
  _preset = preset;
  _presetRepeatRemain = repeat; // 0 ⇒ infinite
  _step = _sub = _burst = 0;

  // Kick off first step immediately
  const unsigned long now = millis();
  switch (_preset) {
    case BuzzerPreset::Init:       on(); _nextToggleMs = now + 1000; break;
    case BuzzerPreset::Stop:       on(); _nextToggleMs = now + 100;  _step = 0; break;
    case BuzzerPreset::Warning1:   _burst = 0; _sub = 0; on(); _nextToggleMs = now + 5; break;
    case BuzzerPreset::Warning2:   on(); _nextToggleMs = now + 100;  break;
    case BuzzerPreset::Warning3:   _sub = 0; on(); _nextToggleMs = now + 25; break;

    case BuzzerPreset::Heartbeat:  on(); _nextToggleMs = now + 50;   _step = 0; break;      // ON 50 -> OFF 950
    case BuzzerPreset::ErrorAlert: on(); _nextToggleMs = now + 200;  _step = 0; _sub = 0; break; // 3 beeps then 1s pause
    case BuzzerPreset::Success:    on(); _nextToggleMs = now + 100;  _step = 0; break;      // ON100, OFF50, ON400
    case BuzzerPreset::Alarm:      on(); _nextToggleMs = now + 2000; _step = 0; break;      // ON2000, OFF200
    case BuzzerPreset::MorseSOS:   // initialize SOS state machine
      _burst = 0; _sub = 0; _step = 0; // _burst = which letter (0=S1,1=O,2=S2), _sub = element index
      on(); _nextToggleMs = now + 100; // start with a dot (100ms)
      break;
  }
}

void Buzzer::stopPreset() {
  if (!_initFlag) { _lastError = BuzzerError::NotInitialized; return; }
  if (_mode == Mode::Preset) {
    _mode = Mode::Idle;
    off();
  }
}

// ------------------------------ Scheduler ------------------------------
void Buzzer::update() {
  if (!_initFlag) return;

  const unsigned long now = millis();
  if (_mode == Mode::Idle) return;
  if ((long)(now - _nextToggleMs) < 0) return;

  switch (_mode) {
    // -------- BLINK ENGINE --------
    case Mode::Blink: {
      if (_blinkOnPhase) {
        // ON -> OFF
        off();
        _blinkOnPhase = false;
        _nextToggleMs = now + _blinkOffMs;
      } else {
        // OFF -> next ON or stop if finite done
        if (_blinkRemain == 1) {
          _mode = Mode::Idle;
          off();
          return;
        }
        if (_blinkRemain > 1) _blinkRemain--; // (0 means infinite)
        on();
        _blinkOnPhase = true;
        _nextToggleMs = now + _blinkOnMs;
      }
      break;
    }

    // -------- PRESET ENGINE --------
    case Mode::Preset: {
      switch (_preset) {
        // Existing presets ----------------------------------------------------
        case BuzzerPreset::Init: {
          if (_currentOn) { off(); _nextToggleMs = now + 100; }
          else {
            if (_presetRepeatRemain == 1) { _mode = Mode::Idle; off(); return; }
            if (_presetRepeatRemain > 1)  _presetRepeatRemain--;
            on(); _nextToggleMs = now + 1000;
          }
        } break;

        case BuzzerPreset::Stop: {
          // (ON100, OFF100) × 2
          if (_step % 2 == 0) { // after ON
            off(); _nextToggleMs = now + 100; _step++;
          } else {              // after OFF
            if (_step == 3) {
              if (_presetRepeatRemain == 1) { _mode = Mode::Idle; off(); return; }
              if (_presetRepeatRemain > 1)  _presetRepeatRemain--;
              _step = 0; on(); _nextToggleMs = now + 100;
            } else {
              on(); _nextToggleMs = now + 100; _step++;
            }
          }
        } break;

        case BuzzerPreset::Warning1: {
          // 10 bursts: each burst 20×(ON5,OFF5), then 200ms pause
          if (_sub < 20) {
            if (_currentOn) { off(); _nextToggleMs = now + 5; }
            else            { on();  _nextToggleMs = now + 5; _sub++; }
          } else {
            if (_currentOn) { off(); _nextToggleMs = now + 200; }
            else {
              _sub = 0;
              if (++_burst >= 10) {
                if (_presetRepeatRemain == 1) { _mode = Mode::Idle; off(); return; }
                if (_presetRepeatRemain > 1)  _presetRepeatRemain--;
                _burst = 0;
              }
              on(); _nextToggleMs = now + 5;
            }
          }
        } break;

        case BuzzerPreset::Warning2: {
          if (_currentOn) { off(); _nextToggleMs = now + 100; }
          else {
            if (_presetRepeatRemain == 1) { _mode = Mode::Idle; off(); return; }
            if (_presetRepeatRemain > 1)  _presetRepeatRemain--;
            on(); _nextToggleMs = now + 100;
          }
        } break;

        case BuzzerPreset::Warning3: {
          if (_sub < 10) {
            if (_currentOn) { off(); _nextToggleMs = now + 25; }
            else            { on();  _nextToggleMs = now + 25; _sub++; }
          } else {
            if (_presetRepeatRemain == 1) { _mode = Mode::Idle; off(); return; }
            if (_presetRepeatRemain > 1)  _presetRepeatRemain--;
            _sub = 0; on(); _nextToggleMs = now + 25;
          }
        } break;

        // New presets ---------------------------------------------------------
        case BuzzerPreset::Heartbeat: {
          // ON 50 -> OFF 950 repeating
          if (_currentOn) { off(); _nextToggleMs = now + 950; }
          else {
            if (_presetRepeatRemain == 1) { _mode = Mode::Idle; off(); return; }
            if (_presetRepeatRemain > 1)  _presetRepeatRemain--;
            on(); _nextToggleMs = now + 50;
          }
        } break;

        case BuzzerPreset::ErrorAlert: {
          // (ON200, OFF100) × 3, then 1000ms pause
          // _sub = which beep inside the triple (0..2)
          if (_currentOn) {
            off(); _nextToggleMs = now + 100;   // inter-beep gap
          } else {
            if (_sub < 2) {
              // next beep
              _sub++;
              on(); _nextToggleMs = now + 200;
            } else {
              // finished 3 beeps -> long pause then repeat/stop
              _sub = 0;
              _nextToggleMs = now + 1000; // keep OFF during pause
              if (_presetRepeatRemain == 1) { _mode = Mode::Idle; off(); return; }
              if (_presetRepeatRemain > 1)  _presetRepeatRemain--;
              // After pause, start next sequence with ON200
              // handled by next update() when time elapses:
            }
          }
        } break;

        case BuzzerPreset::Success: {
          // ON100, OFF50, ON400 (then stop/repeat)
          if (_step == 0) {      // finished ON100
            off(); _nextToggleMs = now + 50;  _step = 1;
          } else if (_step == 1) { // finished OFF50
            on();  _nextToggleMs = now + 400; _step = 2;
          } else {                 // finished ON400 -> OFF and handle repeat
            off();
            if (_presetRepeatRemain == 1) { _mode = Mode::Idle; return; }
            if (_presetRepeatRemain > 1)  _presetRepeatRemain--;
            // restart
            _step = 0;
            on(); _nextToggleMs = now + 100;
          }
        } break;

        case BuzzerPreset::Alarm: {
          // ON2000, OFF200 repeating
          if (_currentOn) {
            off(); _nextToggleMs = now + 200;
          } else {
            if (_presetRepeatRemain == 1) { _mode = Mode::Idle; off(); return; }
            if (_presetRepeatRemain > 1)  _presetRepeatRemain--;
            on(); _nextToggleMs = now + 2000;
          }
        } break;

        case BuzzerPreset::MorseSOS: {
          // Timing unit = 100ms
          // _burst = letter index (0=S, 1=O, 2=S), _sub = element index in letter
          // For S: 3 dots; for O: 3 dashes.
          auto startDot  = [&](){ on();  _nextToggleMs = now + 100; };
          auto startDash = [&](){ on();  _nextToggleMs = now + 300; };

          const uint8_t letter = _burst; // 0,1,2
          const bool isS = (letter != 1);
          const uint8_t needed = 3; // elements per letter

          if (_currentOn) {
            // turn OFF after element ON time; inter-element gap 100ms
            off(); _nextToggleMs = now + 100;

            // advance element counter only after ON finished
            if (++_sub >= needed) {
              // finished a letter; after gap of (already 100ms), add +200ms more to make ~300ms char gap
              _nextToggleMs = now + 200; // total ~300 including the 100ms just set
              _sub = 0;
              if (++_burst >= 3) {
                // finished SOS
                if (_presetRepeatRemain == 1) { _mode = Mode::Idle; off(); return; }
                if (_presetRepeatRemain > 1)  _presetRepeatRemain--;
                _burst = 0; // next repeat from first letter
              }
            }
          } else {
            // currently OFF and gap elapsed: start next element of current letter
            if (isS) startDot();
            else     startDash();
          }
        } break;
      }
    } break;

    case Mode::Idle: default: break;
  }
}
