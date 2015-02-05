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

void loop() {
    turboState = digitalRead(turboSwitch);
    if (turboState == LOW) {
        digitalWrite(LED, 1);
    } else {
        digitalWrite(LED, 0);
    }

    // Map like a Super Nintendo
    mapButton(faceButtonRight, 'a');
    mapButton(faceButtonUp, 'y');
    mapButton(faceButtonLeft, 'x');
    mapButton(faceButtonDown, 'b');

    // Use arrow keys
    mapButton(dPadUp, KEY_UP_ARROW);
    mapButton(dPadLeft, KEY_LEFT_ARROW);
    mapButton(dPadDown, KEY_DOWN_ARROW);
    mapButton(dPadRight, KEY_RIGHT_ARROW);

    mapButton(leftShoulderButton, 'l');
    mapButton(rightShoulderButton, 'r');

    mapButton(leftJoystickButton, 'q');
    mapButton(rightJoystickButton, 'p');

    delay(1);        // delay in between reads for stability
}
