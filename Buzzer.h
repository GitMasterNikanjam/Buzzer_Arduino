#pragma once
#include <Arduino.h>

/**
 * @file Buzzer.h
 * @brief Buzzer driver with finite/infinite blink and blocking & non-blocking BuzzerPreset sounds.
 *
 * - Direct control: on(), off(), isOn()
 * - Blink engine: startBlink(on_ms, off_ms, repeat) where repeat==0 ⇒ infinite
 * - BuzzerPreset sounds: playPreset(BuzzerPreset, non_blocking, repeat)
 * - Non-blocking requires calling update() frequently (e.g., each loop()).
 *
 * Error handling is zero-allocation via ::BuzzerError (no String).
 */

/** @enum BuzzerError
 *  @brief Error codes (no dynamic allocation).
 */
enum class BuzzerError : uint8_t {
  None = 0,          ///< No error
  InvalidPin,        ///< PIN_NUM < 0 or unsupported
  InvalidActiveMode, ///< ACTIVE_MODE must be 0 or 1
  NotInitialized     ///< Operation requires successful init()
};

/** @enum BuzzerPreset
 *  @brief BuzzerPreset audible patterns.
 */
enum class BuzzerPreset : uint8_t {
  Init = 0,    ///< 1s ON, 0.1s OFF
  Stop,        ///< Two short beeps
  Warning1,    ///< 10 bursts; each burst: 20×(5ms ON, 5ms OFF) then 200ms pause
  Warning2,    ///< 100ms ON, 100ms OFF
  Warning3,    ///< 10×(25ms ON, 25ms OFF)

  Heartbeat,   ///< Short pip every ~1s: ON 50ms, OFF 950ms (repeatable)
  ErrorAlert,  ///< Triple: (ON 200ms, OFF 100ms)×3, then 1s pause
  Success,     ///< Confirmation: ON 100ms, OFF 50ms, ON 400ms (once per repeat)
  Alarm,       ///< Long ON siren-like: ON 2000ms, OFF 200ms (repeatable)
  MorseSOS     ///< SOS in Morse (· · · — — — · · ·), 100ms unit timing
};

/**
 * @class Buzzer
 * @brief Digital buzzer driver (active-HIGH or active-LOW).
 *
 * @code
 * Buzzer bz;
 * bz.parameters.PIN_NUM = 8;
 * bz.parameters.ACTIVE_MODE = 1;       // active-HIGH
 * if (!bz.init()) {
 *   Serial.begin(115200);
 *   Serial.println(Buzzer::errorToCstr(bz.lastError()));
 *   while (1) {}
 * }
 *
 * // Example 1: infinite non-blocking blink (100ms ON / 900ms OFF)
 * bz.startBlink(100, 900, 0);
 *
 * // Example 2: play BuzzerPreset in non-blocking mode, repeat 3 times
 * bz.playPreset(BuzzerPreset::ErrorAlert, /non_blocking=/true, /repeat=/3);
 *
 * void loop() {
 *   bz.update(); // must be called often for non-blocking modes
 *   // ... your real-time work ...
 * }
 * @endcode
 */
class Buzzer {
public:
  /** @struct ParametersStruct
   *  @brief User-configurable parameters.
   */
  struct ParametersStruct {
    int     PIN_NUM     = -1; ///< GPIO pin number (>=0)
    uint8_t ACTIVE_MODE = 0;  ///< 1: active-HIGH, 0: active-LOW
  } parameters;

  // -------------------- Lifetime --------------------
  /** @brief Construct with defaults (PIN_NUM=-1, ACTIVE_MODE=0). */
  Buzzer() = default;

  /** @brief Destructor: return the pin to INPUT if initialized. */
  ~Buzzer();

  // -------------------- Init & errors --------------------
  /**
   * @brief Initialize hardware; sets OUTPUT and OFF.
   * @return true on success, false on invalid parameters.
   */
  bool init();

  /** @brief Whether init() succeeded. */
  bool getInitFlag() const { return _initFlag; }

  /** @brief Get last error (sticky until cleared or a success path). */
  BuzzerError lastError() const { return _lastError; }

  /** @brief Clear last error (sets to ::BuzzerError::None). */
  void clearError() { _lastError = BuzzerError::None; }

  /**
   * @brief Convert error code to a static C-string (no allocation).
   * @param e Error code
   * @return Null-terminated description
   */
  static const char* errorToCstr(BuzzerError e);

  // -------------------- Direct control --------------------
  /** @brief Turn buzzer ON (per ACTIVE_MODE). Requires init(). */
  void on();

  /** @brief Turn buzzer OFF (inverse of ACTIVE_MODE). Requires init(). */
  void off();

  /** @brief Software-tracked ON/OFF state. */
  bool isOn() const { return _currentOn; }

  // -------------------- BuzzerPreset sounds (blocking, one pass per call) --------------------
  /** @brief One long tone (startup OK). */
  void soundInit();
  /** @brief Two short beeps (stop/abort). */
  void soundStop();
  /** @brief Rapid beeps pattern (warning #1). */
  void soundWarning_1();
  /** @brief Short on/off pulse (warning #2). */
  void soundWarning_2();
  /** @brief Short rapid burst (warning #3). */
  void soundWarning_3();

  /** @brief Heartbeat: ON 50ms, OFF 950ms (one cycle). */
  void soundHeartbeat();
  /** @brief ErrorAlert: (ON 200ms, OFF 100ms)×3 then 1s pause (one sequence). */
  void soundErrorAlert();
  /** @brief Success: ON 100ms, OFF 50ms, ON 400ms (one sequence). */
  void soundSuccess();
  /** @brief Alarm: ON 2000ms, OFF 200ms (one cycle). */
  void soundAlarm();
  /** @brief Morse SOS: '... --- ...' with 100ms unit timing (one sequence). */
  void soundMorseSOS();

  // -------------------- Blink (non-blocking) --------------------
  /**
   * @brief Start non-blocking blink.
   * @param on_ms   ON time in ms
   * @param off_ms  OFF time in ms
   * @param repeat  Number of ON pulses; 0 ⇒ infinite
   * @note Call update() frequently.
   */
  void startBlink(uint16_t on_ms, uint16_t off_ms, uint32_t repeat = 0);

  /** @brief Stop any running non-blocking blink; forces OFF. */
  void stopBlink();

  // -------------------- BuzzerPreset sounds (blocking or non-blocking) --------------------
  /**
   * @brief Play a BuzzerPreset sound in blocking or non-blocking mode.
   * @param preset       Which BuzzerPreset to play
   * @param non_blocking true ⇒ run via update(); false ⇒ run blocking here
   * @param repeat       How many times to repeat the BuzzerPreset (0 ⇒ infinite in non-blocking mode)
   *
   * In blocking mode, repeat must be >= 1.
   */
  void playPreset(BuzzerPreset preset, bool non_blocking, uint32_t repeat = 1);

  /** @brief Stop any running non-blocking BuzzerPreset; forces OFF. */
  void stopPreset();

  /// @return true if a non-blocking blink/preset is currently running.
  bool isBusy() const { return _mode != Mode::Idle; }

  /// @brief Stop any activity (blink or preset) and turn OFF.
  void stopAll() { stopBlink(); stopPreset(); }

  // -------------------- Scheduler --------------------
  /** @brief Advance non-blocking engines (blink/BuzzerPreset). Call from loop(). */
  void update();

private:
  // ---- helpers / validation ----
  bool _checkParameters();
  inline uint8_t _onLevel()  const { return parameters.ACTIVE_MODE ? HIGH : LOW;  }
  inline uint8_t _offLevel() const { return parameters.ACTIVE_MODE ? LOW  : HIGH; }

  // ---- global engine mode ----
  enum class Mode : uint8_t { Idle = 0, Blink, Preset };
  Mode _mode = Mode::Idle;

  // ---- common state ----
  bool        _initFlag  = false;
  bool        _currentOn = false;
  BuzzerError _lastError = BuzzerError::None;

  // ---- blink state ----
  bool           _blinkOnPhase = false;
  uint16_t       _blinkOnMs = 0;
  uint16_t       _blinkOffMs = 0;
  uint32_t       _blinkRemain = 0;     ///< Remaining ON pulses; 0 ⇒ infinite
  unsigned long  _nextToggleMs = 0;

  // ---- BuzzerPreset state (non-blocking) ----
  BuzzerPreset         _preset = BuzzerPreset::Init;
  uint32_t       _presetRepeatRemain = 0;  ///< Remaining preset repeats; 0 ⇒ infinite

  // Fine-grained sub-state for complex patterns:
  uint8_t        _step = 0;     ///< generic step index inside a preset
  uint8_t        _sub  = 0;     ///< inner loop counter (e.g., pulses)
  uint8_t        _burst= 0;     ///< outer loop counter (for Warning1 / SOS groups)
};
