#include <Arduino.h>

// =======================
// PIN DEFINITIONS
// =======================
#define A GPIO_NUM_18
#define B GPIO_NUM_19
#define X GPIO_NUM_22
#define Y GPIO_NUM_23
#define LB GPIO_NUM_27
#define RB GPIO_NUM_4
#define START GPIO_NUM_21 

#define PEDAL_UP    GPIO_NUM_26
#define PEDAL_DOWN  GPIO_NUM_25
#define PEDAL_LEFT  GPIO_NUM_36 // was 14, changed due to weird analog glitch with lb and rb
#define PEDAL_RIGHT GPIO_NUM_32

#define CSTICK_X    GPIO_NUM_15 
#define CSTICK_Y    GPIO_NUM_33

#define LT GPIO_NUM_34
#define RT GPIO_NUM_35

// =======================
// CALIBRATION (Adjusted for Feet-On-Pedals)
// =======================

// --- PEDAL RIGHT (Active Low) ---
// Log: Rest ~1822, Max Press ~1703. Short range (~120).
// Drift seen: Positive 0.25 (meaning value dropped below 1800).
// New Rest: 1760 (Pushing threshold lower to ignore foot weight)
#define RIGHT_REST 1760 
#define RIGHT_MAX  1710 

// --- PEDAL LEFT (Active High) ---
// Log: Rest ~2080.
// New Rest: 2200 (Raised to ignore foot weight)
#define LEFT_REST 2200  
#define LEFT_MAX  2800

// --- PEDAL UP (Active High) ---
// Log: Rest ~1983.
// New Rest: 2100 (Raised to ignore foot weight)
#define UP_REST   2050  
#define UP_MAX    2400  

// --- PEDAL DOWN (Active High) ---
// Log: Rest ~1920.
// Drift seen: Negative 0.08 (meaning value went above 2000).
// New Rest: 2100 (Raised to ignore foot weight)
#define DOWN_REST 2050  
#define DOWN_MAX  2400

// --- JOYSTICK & TRIGGERS ---
#define JOY_CENTER 1850 
#define JOY_DEADZONE 250
#define JOY_MIN 0
#define JOY_MAX 4095

#define LT_MIN 2160  
#define LT_MAX 2750
#define RT_MIN 2160
#define RT_MAX 2800

#define TARGET_MAX 255

// =======================
// VARIABLES
// =======================
unsigned long lastLoopTime = 0;
const int loopInterval = 10; 
int prevStartReading = -1;       
bool virtualStartButtonState = false; 
unsigned long startButtonTimer = 0;   
const int startButtonDuration = 100;  

void setup() {
  Serial.begin(115200);

  // Buttons
  pinMode(A, INPUT_PULLDOWN);
  pinMode(B, INPUT_PULLDOWN);
  pinMode(X, INPUT_PULLDOWN);
  pinMode(Y, INPUT_PULLDOWN);
  pinMode(LB, INPUT_PULLDOWN);
  pinMode(RB, INPUT_PULLDOWN);
  pinMode(START, INPUT_PULLDOWN);

  // Pedals
  pinMode(PEDAL_UP, INPUT);
  pinMode(PEDAL_DOWN, INPUT);
  pinMode(PEDAL_LEFT, INPUT);
  pinMode(PEDAL_RIGHT, INPUT);

  // C-Stick & Triggers
  pinMode(CSTICK_X, INPUT);
  pinMode(CSTICK_Y, INPUT);
  pinMode(LT, INPUT);
  pinMode(RT, INPUT);
}

// Helper: Active High Mapping (Value goes UP)
int mapActiveHigh(int val, int rest, int max_val) {
    if (val < rest) return 0; // Deadzone
    int mapped = map(val, rest, max_val, 0, 127);
    return constrain(mapped, 0, 127);
}

// Helper: Active Low Mapping (Value goes DOWN) - Specifically for Right Pedal
int mapActiveLow(int val, int rest, int max_val) {
    if (val > rest) return 0; // Deadzone (Value is higher than threshold)
    // Map from Rest(High) -> Max(Low) to 0 -> 127
    int mapped = map(val, rest, max_val, 0, 127);
    return constrain(mapped, 0, 127);
}

int mapJoystick(int val) {
    int centered = val - JOY_CENTER;
    if (abs(centered) < JOY_DEADZONE) return 0;
    int result = 0;
    if (centered > 0) result = map(centered, JOY_DEADZONE, (JOY_MAX - JOY_CENTER), 0, 127);
    else result = map(centered, -JOY_DEADZONE, -(JOY_CENTER - JOY_MIN), 0, -127);
    return constrain(result, -127, 127);
}

int mapTrigger(int val, int min_t, int max_t) {
    if (val < min_t) return 0;
    long mapped = map(val, min_t, max_t, 0, TARGET_MAX);
    return constrain(mapped, 0, TARGET_MAX);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastLoopTime >= loopInterval) {
    lastLoopTime = currentMillis;

    // --- START BUTTON ---
    int currentStartReading = digitalRead(START);
    if (currentStartReading != prevStartReading) {
      if (prevStartReading != -1) { 
        virtualStartButtonState = true; 
        startButtonTimer = currentMillis; 
      }
      prevStartReading = currentStartReading; 
    }
    if (virtualStartButtonState && (currentMillis - startButtonTimer > startButtonDuration)) {
      virtualStartButtonState = false; 
    }

    // --- BUTTONS ---
    int b_a = digitalRead(A);
    int b_b = digitalRead(B);
    int b_x = digitalRead(X);
    int b_y = digitalRead(Y);
    int b_lb = digitalRead(LB);
    int b_rb = digitalRead(RB);

    // --- PEDALS ---
    int valRight = mapActiveLow(analogRead(PEDAL_RIGHT), RIGHT_REST, RIGHT_MAX);
    int valLeft  = mapActiveHigh(analogRead(PEDAL_LEFT), LEFT_REST, LEFT_MAX);
    int valUp    = mapActiveHigh(analogRead(PEDAL_UP), UP_REST, UP_MAX);
    int valDown  = mapActiveHigh(analogRead(PEDAL_DOWN), DOWN_REST, DOWN_MAX);

    // Combine into Axes
    int lx = valRight - valLeft; 
    int ly = valUp - valDown;

    // --- C-STICK ---
    int cx = mapJoystick(analogRead(CSTICK_X));
    // int cx = analogRead(CSTICK_X);
    int cy = mapJoystick(analogRead(CSTICK_Y));
    // int cy = analogRead(CSTICK_Y); 

    // --- TRIGGERS ---
    int trig_l = mapTrigger(analogRead(LT), LT_MIN, LT_MAX);
    int trig_r = mapTrigger(analogRead(RT), RT_MIN, RT_MAX);

    // Serial.println("CX: " + String(cx) + " CY: " + String(cy));
    // --- SEND DATA ---
    Serial.printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", 
      virtualStartButtonState, b_a, b_b, b_x, b_y, b_lb, b_rb, lx, ly, cy, cx, trig_l, trig_r);
  }
}