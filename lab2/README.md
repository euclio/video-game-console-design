# E190U Lab 2: <small>Gamepad Firmware</small>
<address>Andy Russell</address>

## Introduction

In this lab, I programmed the firmware for the Arduino micro board on the video
game controller that I designed in [lab1](../lab1/README.md). The firmware was
written in C++. It supports full keyboard-mouse interaction and a turbo switch
across ten buttons, two joysticks, and a switch.

## Design Methodology

In the last lab, I constructed the hardware for the controller, but many of the
features that I wanted to give my controller required software support from the
Arduino board.

With this lab, I really wanted to make the firmware extensible and easy to
modify. To my surprise, a lot of games do not support remapping their keys.
To mitigate this I tried to reuse as much code as possible between each button
mapping so that changing keys is as easy as modifying a single character.

To this end, I defined a number of functions that encapsulate the functionality
of reading the pins. I marked these functions `inline` to avoid the overhead of
the numerous function calls that this introduces. Here's the function that maps
a mouse button press to a digital input:

```cpp
inline void mapMouseButton(pin pin, int mouseButton) {
    int buttonState = digitalRead(pin);

    if (buttonState == LOW) {
        Mouse.press(mouseButton);
    } else {
        Mouse.release(mouseButton);
    }
}
```

This function is called once per loop. If the pin stays low, then the mouse
button will remain pressed. Once the pin reads high, then the mouse is released.
This ensures that the while the pin stays low between loops is will appear that
the mouse is being held down.

I use a similar method for mapping buttons, but I also take the state of the
turbo switch into account.

```cpp
inline void mapButton(pin pin, char key) {
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
```

This case operates in a very similar way, however if the turbo switch is enabled
then the keyboard key is released immediately after it is pressed. Therefore, if
the button is held on the controller while the turbo switch is enabled, it will
appear that the switch is being pressed very rapidly.

The advantage of putting this functionality into functions means that the main
loop of my firmware acts like a button map. I can call these functions like so:

```cpp
mapButton(faceButtonRight, 'z');
mapMouseButton(rightJoystickButton, MOUSE_RIGHT);
```

and easily remap the pins or keys at any time.



## Testing Methodology

There were two main concerns I had with testing: making sure that the buttons
operated as expected and that the controls felt natural. In order to accomplise
the first goal, I made extensive use of the serial feature of the Arduino micro.
This allowed me to see the values as they were being reported directly from the
board. To make sure the controls felt natural, I played various Steam games and
Lugaru. Playing these games helped me determine whether I felt like I was
"fighting" my controller or not.

There were a surprising number of games that did not support remapping controls
or multiple keyboards. In many cases, I modified my firmware to allow me to play
these games with their intended controls.

## Results and Discussion

I used [Arduino CMake](https://github.com/queezythegreat/arduino-cmake) for this
project, which allowed me to use my own editor instead of the Arduino IDE. The
firmware is uploaded to the Arduino board with the following code:

```bash
mkdir build && cd build
cmake ..
make upload
```

You must press the reset button before uploading the firmware.

To view the serial output, you may use `make controller_firmware-serial`. This
will open the serial tty in `screen` in your terminal. If the terminal does not
work, try entering the root directory and entering ``stty < /dev/ttyACM0 `cat
arduino.ttysettings` ``.

I am happy with my firmware code because I think it meets my original goal of
ease of modification. I really put that feature to the test while I was testing
my controller with Steam games.

## Conclusions

Overall, I spent about 5 hours programming the firmware. I had never used an
Arduino before, so I spent a lot of time trying to figure out how to build the
libraries without the IDE and upload the firmware manually. However, I am very
satisfied with my results. In addition to the buttons registering keypressed
correctly, I am also satisfied with the mouse sensitivity.

My firmware code may be found [here](src/controller_firmware.cpp).
