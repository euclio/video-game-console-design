/*
 * Firmware for a video game controller. The controller has 10 buttons, two
 * joysticks, and a turbo switch.
 */

#include "Arduino.h"
#include "SoftwareSerial.h"

#ifndef SERIAL_BAUD
#define SERIAL_BAUD 9800
#endif

// Mappings from buttons to pins
const int faceButtonRight = 2;
const int faceButtonUp = 3;
const int faceButtonLeft = 4;
const int faceButtonDown = 5;

const int dPadRight = 6;
const int dPadUp = 7;
const int dPadLeft = 8;
const int dPadDown = 9;

const int rightShoulderButton = 10;
const int leftShoulderButton = 11;

const int turboSwitch = 12;

const int leftJoystickX = A0;
const int leftJoystickY = A1;
const int leftJoystickButton = A2;

const int rightJoystickX = A3;
const int rightJoystickY = A4;
const int rightJoystickButton = A5;

const int LED = 13;

// The largest and smallest possible values from an analog reading.
const int analogMin = 0;
const int analogMax = 1023;

// The required difference from resting position before an analog reading
// should register. Since the analog sticks will likely not rest in the middle
// of the analog range, this threshold can be used to ignore noise.
const int analogThreshold = 50;

// The scale factor between the analog reads and the amount of pixels the mouse
// will move. It must be less than .25 because the maximum value of an analog
// read is 1024 and the mouse position must fit inside a byte.
const float mouseSensitivity = 0.10;

// The state of the turbo switch
int turboState;

void setup() {
    // Initialize serial communication.
    Serial.begin(SERIAL_BAUD);

    // Set all digital pins to use the internal pullup resistors.
    pinMode(faceButtonRight, INPUT_PULLUP);
    pinMode(faceButtonUp, INPUT_PULLUP);
    pinMode(faceButtonLeft, INPUT_PULLUP);
    pinMode(faceButtonDown, INPUT_PULLUP);

    pinMode(dPadRight, INPUT_PULLUP);
    pinMode(dPadUp, INPUT_PULLUP);
    pinMode(dPadLeft, INPUT_PULLUP);
    pinMode(dPadDown, INPUT_PULLUP);

    pinMode(leftShoulderButton, INPUT_PULLUP);
    pinMode(rightShoulderButton, INPUT_PULLUP);

    pinMode(turboSwitch, INPUT_PULLUP);

    // Indicator that the turbo switch is on.
    pinMode(LED, OUTPUT);

    // Initialize moust and keyboard control.
    Mouse.begin();
    Keyboard.begin();
}

/*
 * Maps a pin input to a specific keyboard input.
 *
 * Given a pin, if the pin is low then register a key press, otherwise the key
 * is released.
 */
inline void mapButton(int pin, char key) {
    int buttonState = digitalRead(pin);

    if (buttonState == LOW) {
        Keyboard.press(key);

        // Release the key anyways if turbo is enabled. This makes holding
        // a button simulate many quick presses.
        if (turboState == LOW) {
            Keyboard.release(key);
        }
    } else {
        Keyboard.release(key);
    }
}

/*
 * Defines keys that should be pressed once the analog pin's reading surpasses
 * the analog threshold for either the minimum or maximum analog value.
 *
 * Note: Turbo is consciously NOT enabled for this function, as they joysticks
 * are likely to be used for movement or camera control, which means that turbo
 * would likely only serve as a hindrance.
 */
inline void mapAnalogKey(int pin, char keyMin, char keyMax) {
    int pinValue = analogRead(pin);

    if (pinValue < analogMin + analogThreshold) {
        Keyboard.press(keyMin);
        Keyboard.release(keyMax);
    } else if (pinValue > analogMax - analogThreshold) {
        Keyboard.press(keyMax);
        Keyboard.release(keyMin);
    } else {
        Keyboard.release(keyMin);
        Keyboard.release(keyMax);
    }
}

/*
 * Defines which mouse button should be pressed when a pin reads low. Works
 * similarly to mapButton, though turbo is not enabled.
 */
inline void mapMouseButton(int pin, int mouseButton) {
    int buttonState = digitalRead(pin);

    if (buttonState == LOW) {
        Mouse.press(mouseButton);
    } else {
        Mouse.release(mouseButton);
    }
}

/*
 * Defines the analog pins that should move the mouse.
 */
inline void mapMouseMove(int xAxisPin, int yAxisPin) {
    const int analogMid = analogMax / 2;

    // Get the difference between the potentiometer readings and the absolute
    // middle of the analog range.
    int dx = analogRead(xAxisPin) - analogMid;
    int dy = analogMid - analogRead(yAxisPin);

    // If the reading is greater than the threshold, then we will move the
    // mouse by the reading (multiplied by the sensitivity). If it's not, then
    // we don't move at all to prevent the mouse from drifting when the sticks
    // are resting.
    int xMovement = abs(dx) > analogThreshold ? dx * mouseSensitivity : 0;
    int yMovement = abs(dy) > analogThreshold ? dy * mouseSensitivity : 0;
    Mouse.move(xMovement, yMovement, 0);
}

void loop() {
    turboState = digitalRead(turboSwitch);
    if (turboState == LOW) {
        digitalWrite(LED, HIGH);
    } else {
        digitalWrite(LED, LOW);
    }

    mapButton(faceButtonRight, 'z');
    mapButton(faceButtonUp, 'x');
    mapButton(faceButtonLeft, 'c');
    mapButton(faceButtonDown, 'v');

    // Use arrow keys
    mapButton(dPadUp, KEY_UP_ARROW);
    mapButton(dPadLeft, KEY_LEFT_ARROW);
    mapButton(dPadDown, KEY_DOWN_ARROW);
    mapButton(dPadRight, KEY_RIGHT_ARROW);

    mapButton(leftShoulderButton, 'l');
    mapButton(rightShoulderButton, 'r');

    // Use WASD
    mapAnalogKey(leftJoystickX, 'a', 'd');
    mapAnalogKey(leftJoystickY, 's', 'w');
    mapMouseButton(leftJoystickButton, MOUSE_LEFT);

    mapMouseMove(rightJoystickX, rightJoystickY);
    mapMouseButton(rightJoystickButton, MOUSE_RIGHT);

    delay(1);        // delay in between reads for stability
}
