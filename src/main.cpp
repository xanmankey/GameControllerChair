#include <Arduino.h>
// #include <BleCompositeHID.h>
#include <BleGamepad.h>
// #include "GamepadDevice.h"
#include <WiFi.h>
#include <string.h>

// 17 GPIO pins
// Pinout for MCU at https://mischianti.org/wp-content/uploads/2021/03/ESP32-DOIT-DEV-KIT-v1-pinout-mischianti-1536x752.jpg
// AVOID USING
// GPIO_0 (used for boot), GPIO_1 and GPIO_3 (used for flashing and code upload)
// NOTE:
// GPIO_34, GPIO_35, GPIO_36, and GPIO_39 are input only pins and do not have internal resistors
// ALSO AVOID USING GPIO_21 AT ALL COSTS: I WROTE THIS TO SCARE YOU AND THE PIN IS FINE

#define A GPIO_NUM_18
#define B GPIO_NUM_19
#define X GPIO_NUM_22
#define Y GPIO_NUM_23

#define UP GPIO_NUM_26
#define DOWN GPIO_NUM_25
#define LEFT GPIO_NUM_14
#define RIGHT GPIO_NUM_32

#define CUP_DOWN GPIO_NUM_33
// #define CDOWN GPIO_NUM_5
#define CLEFT_RIGHT GPIO_NUM_15
// #define CRIGHT GPIO_NUM_16

#define LB GPIO_NUM_27
#define RB GPIO_NUM_4

#define LT GPIO_NUM_34
#define RT GPIO_NUM_35

#define START GPIO_NUM_21

#define NUM_BUTTONS 7

// Analog Stick, C stick, and triggers are not considered buttons by https://github.com/lemmingDev/ESP32-BLE-Gamepad
// and will be handled as axes
// Adjust parameters as needed for gameplay feel
#define HALL_EFFECT_MIN_PRESSED 0 // Full magnet value
#define HALL_EFFECT_DEADZONE 200 // Dead zone to avoid jitter; adjust as needed
#define TRIGGER_MAX 255 // Standard 8-bit trigger range
#define ANALOG_STICK_MAX 32767 // Standard signed 16-bit range for sticks

// TODO: start button should be ok, but pedals and triggers still need to be figured out
#define LT_REST 3550
#define RT_REST 3760
#define X_REST 250
#define Y_REST 1600

BleGamepad gamepad("CHAIR", "plugNplay", 100);
// GamepadDevice* gamepad;
// GamepadConfiguration bleGamepadConfig;

byte previousInputStates[NUM_BUTTONS];
byte currentInputStates[NUM_BUTTONS];
byte buttonPins[NUM_BUTTONS] = {A, B, X, Y, LB, RB, START};
// NOTE: not sure what BUTTON_1-6 display as; there may be better choices to select for this
byte physicalButtons[NUM_BUTTONS] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7};
bool start = false;

void setup() {
  Serial.begin(115200);
  // Disable wifi as a power optimization
  WiFi.mode(WIFI_OFF);

  while (!Serial) {
    delay(1); // will pause Zero, Leonardo, etc until serial console opens
  }
  Serial.println("Starting...");

  // Buttons
  pinMode(A, INPUT_PULLDOWN);
  pinMode(B, INPUT_PULLDOWN);
  pinMode(X, INPUT_PULLDOWN);
  pinMode(Y, INPUT_PULLDOWN);
  pinMode(LB, INPUT_PULLDOWN);
  pinMode(RB, INPUT_PULLDOWN);
  pinMode(START, INPUT_PULLDOWN);
  pinMode(RT, INPUT); // Analog
  pinMode(LT, INPUT); // Analog

  // Analog axes
  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(CUP_DOWN, INPUT);
  pinMode(CLEFT_RIGHT, INPUT);

  // Gamepad configuration
  // bleGamepadConfig.setAutoReport(false);
  // bleGamepadConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD); // CONTROLLER_TYPE_JOYSTICK, CONTROLLER_TYPE_GAMEPAD (DEFAULT), CONTROLLER_TYPE_MULTI_AXIS
  // bleGamepadConfig.setButtonCount(NUM_BUTTONS);

  // bleGamepadConfig.setWhichAxes(true, true, false, true, true, false, true, true);
  // bleGamepadConfig.setIncludePlayerIndicators(false);

  // bleGamepadConfig.setAxesMin(-32768); // -32768 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal
  // bleGamepadConfig.setAxesMax(32767); // 32767 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal 
  
  // // gamepad = new GamepadDevice(bleGamepadConfig);

  // compositeHID.addDevice(gamepad);
  // compositeHID.begin();
  // gamepad->setAxes(); // Reset all axes to zero before reading
  gamepad.begin();
}

// When designing, make sure all magnetic poles are facing the same direction!
int mapAnalogInput(unsigned int pin, bool isStick, bool isX = false, bool isY = false) {
  unsigned int pinValue = analogRead(pin);

  if (!isStick) {
    // Trigger
    // if (pinValue > HALL_EFFECT_RESTING_TRIGGERS) pinValue = HALL_EFFECT_RESTING_TRIGGERS;
    // if (pinValue < TRIGGER_REST) return 0;
    if (pin == LT) {
      if (pinValue > LT_REST) return 0;
      return map(pinValue, LT_REST, HALL_EFFECT_MIN_PRESSED, 0, TRIGGER_MAX);
    } else if (pin == RT) {
      if (pinValue > RT_REST) return 0;
      return map(pinValue, RT_REST, HALL_EFFECT_MIN_PRESSED, 0, TRIGGER_MAX);
    }
  } else {
    // Analog stick
    if (isX) {
      if (pinValue > X_REST) return 0;
      return map(pinValue, X_REST, HALL_EFFECT_MIN_PRESSED, 0, ANALOG_STICK_MAX);
    } else if (isY) {
      if (pinValue > Y_REST) return 0;
      return map(pinValue, Y_REST, HALL_EFFECT_MIN_PRESSED, 0, ANALOG_STICK_MAX);
    }
  }
}

void loop() {
  if (gamepad.isConnected()) {
    // Read buttons
    for (int i = 0; i < NUM_BUTTONS; i++) {
      currentInputStates[i] = digitalRead(buttonPins[i]);
      if (buttonPins[i] == START) {
        if (currentInputStates[i] != previousInputStates[i]) {
          gamepad.press(physicalButtons[i]);
          delay(100);
        }
        else {
          gamepad.release(physicalButtons[i]);
          // Serial.println("Start released 2");
        }
        previousInputStates[i] = currentInputStates[i];
      }
      else if (currentInputStates[i] != previousInputStates[i]) {
        // Handle start button differently
        if (currentInputStates[i]) {
          gamepad.press(physicalButtons[i]);
        } else {
          gamepad.release(physicalButtons[i]);
        }
        previousInputStates[i] = currentInputStates[i];
      }
    }

    // Serial.print("Buttons: ");
    // for (int i = 0; i < NUM_BUTTONS; i++) {
    //   Serial.print(String(currentInputStates[i]) + " ");
    // }
    // Serial.println("Start current state: " + String(currentInputStates[6]));
    // Serial.println("Start previous state: " + String(previousInputStates[6]));
    // Serial.println("Start: " + String(currentInputStates[6]));

    // Axes initialization
    int32_t x = 0;
    int32_t y = 0;

    int32_t cX = 0;
    int32_t cY = 0;

    int16_t lt = 0;
    int16_t rt = 0;

    // Read analog stick axes
    // TODO: no thresholding for pedals, left and right triggers, start button, making sure axes get pressed in gamepad tester
    x = analogRead(LEFT) - analogRead(RIGHT);
    y = analogRead(UP) - analogRead(DOWN);

    // Read C stick axes
    cX = analogRead(CLEFT_RIGHT);
    cY = analogRead(CUP_DOWN);

    // Read triggers
    lt = analogRead(LT);
    // lt = mapAnalogInput(LT, false);
    rt = analogRead(RT);
    // rt = mapAnalogInput(RT, false);

    Serial.println("X: " + String(x) + " Y: " + String(y) + " C_X: " + String(cX) + " C_Y: " + String(cY) + " LT: " + String(lt) + " RT: " + String(rt));
    delay(1000);

    lt = mapAnalogInput(LT, false);
    rt = mapAnalogInput(RT, false);
    x = mapAnalogInput(LEFT, true, true, false) - mapAnalogInput(RIGHT, true, true, false);
    y = mapAnalogInput(UP, true, false, true) - mapAnalogInput(DOWN, true, false, true);
    cX = map(analogRead(CLEFT_RIGHT), 0, 4095, -ANALOG_STICK_MAX, ANALOG_STICK_MAX);
    cY = map(analogRead(CUP_DOWN), 0, 4095, -ANALOG_STICK_MAX, ANALOG_STICK_MAX);

    delay(1000);
    Serial.println("Mapped X: " + String(x) + " Y: " + String(y) + " C_X: " + String(cX) + " C_Y: " + String(cY) + " LT: " + String(lt) + " RT: " + String(rt));

    // Send axes and triggers to gamepad
    gamepad.setAxes(x, y, 0, cX, cY, 0, lt, rt);
  
  }
  
  delay(10); // Small delay to avoid overwhelming the BLE stack
}