#include <Arduino.h>
#include <BleCompositeHID.h>
#include "GamepadDevice.h"
#include <WiFi.h>
#include <string.h>

// 17 GPIO pins
// Pinout for MCU at https://mischianti.org/wp-content/uploads/2021/03/ESP32-DOIT-DEV-KIT-v1-pinout-mischianti-1536x752.jpg
// AVOID USING
// GPIO_0 (used for boot), GPIO_1 and GPIO_3 (used for flashing and code upload)
// NOTE:
// GPIO_34, GPIO_35, GPIO_36, and GPIO_39 are input only pins and do not have internal resistors
// ALSO AVOID USING GPIO_21 AT ALL COSTS: I WROTE THIS TO SCARE YOU AND THE PIN IS FINE

#define A GPIO_NUM_33
#define B GPIO_NUM_32
#define X GPIO_NUM_25
#define Y GPIO_NUM_26

#define UP GPIO_NUM_23
#define DOWN GPIO_NUM_22
#define LEFT GPIO_NUM_21
#define RIGHT GPIO_NUM_19

#define CUP_DOWN GPIO_NUM_18
// #define CDOWN GPIO_NUM_5
#define CLEFT_RIGHT GPIO_NUM_17
// #define CRIGHT GPIO_NUM_16

#define LB GPIO_NUM_27
#define RB GPIO_NUM_4

#define LT GPIO_NUM_34
#define RT GPIO_NUM_35

#define START GPIO_NUM_14

#define NUM_BUTTONS 7

// Analog Stick, C stick, and triggers are not considered buttons by https://github.com/lemmingDev/ESP32-BLE-Gamepad
// and will be handled as axes
// Adjust parameters as needed for gameplay feel
#define HALL_EFFECT_RESTING 2960 // No magnet value; anything less is 0
#define HALL_EFFECT_RESTING_TRIGGERS 3600 // Larger value for triggers due to mechanical placement
#define HALL_EFFECT_MIN_PRESSED 1100 // Full magnet value
#define HALL_EFFECT_DEADZONE 200 // Dead zone to avoid jitter; adjust as needed
#define TRIGGER_MAX 255 // Standard 8-bit trigger range
#define ANALOG_STICK_MAX 32767 // Standard signed 16-bit range for sticks

BleCompositeHID compositeHID("CHAIR", "plugNplay", 100);
GamepadDevice* gamepad;
GamepadConfiguration bleGamepadConfig;

byte previousInputStates[NUM_BUTTONS];
byte currentInputStates[NUM_BUTTONS];
byte buttonPins[NUM_BUTTONS] = {A, B, X, Y, LB, RB, START};
// NOTE: not sure what BUTTON_1-6 display as; there may be better choices to select for this
byte physicalButtons[NUM_BUTTONS] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6, START_BUTTON};

void setup() {
  Serial.begin(115200);
  // Disable wifi as a power optimization
  WiFi.mode(WIFI_OFF);

  while (!Serial) {
    delay(1); // will pause Zero, Leonardo, etc until serial console opens
  }
  Serial.println("Starting...");

  // Buttons
  pinMode(A, INPUT_PULLUP);
  pinMode(B, INPUT_PULLUP);
  pinMode(X, INPUT_PULLUP);
  pinMode(Y, INPUT_PULLUP);
  pinMode(LB, INPUT_PULLUP);
  pinMode(RB, INPUT_PULLUP);
  pinMode(START, INPUT_PULLUP);
  pinMode(RT, INPUT); // Analog
  pinMode(LT, INPUT); // Analog

  // Axes
}

// When designing, make sure all magnetic poles are facing the same direction!
unsigned int mapAnalogInput(unsigned int pin, bool stick) {
  // Triggers
  unsigned int pinValue = analogRead(pin);
  Serial.println("Raw analog value: " + String(pinValue));
  // Map 0-4095 to 0-255
  if (pinValue > HALL_EFFECT_RESTING + HALL_EFFECT_DEADZONE) {
      pinValue = HALL_EFFECT_RESTING;
  }
  if (stick) {
    // Analog stick
    pinValue = map(pinValue, HALL_EFFECT_RESTING, HALL_EFFECT_MIN_PRESSED, 0, ANALOG_STICK_MAX);
  }
  else {
    // Trigger
    pinValue = map(pinValue, HALL_EFFECT_RESTING_TRIGGERS, HALL_EFFECT_MIN_PRESSED, 0, TRIGGER_MAX);
  }
  return pinValue;
}

void loop() {
  // Serial.println("Looping");
  // Serial.println("A: " + String(digitalRead(A)));
  // Serial.println("B: " + String(digitalRead(B)));
  // Serial.println("X: " + String(digitalRead(X)));
  // Serial.println("Y: " + String(digitalRead(Y)));
  // Serial.println("LB: " + String(digitalRead(LB)));
  // Serial.println("RB: " + String(digitalRead(RB)));
  // Serial.println("START: " + String(digitalRead(START)));
  // Serial.println("LT: " + String(analogRead(LT)));
  Serial.println("Trigger: " + String(mapAnalogInput(RT, false)));
  Serial.println("Stick: " + String(mapAnalogInput(RT, true)));
  delay(100);
}