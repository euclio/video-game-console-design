# E190U Final Project: ChipDJ

## Overview

ChipDJ is a program that uses OpenCV and SFML to display an interface for
playing chiptune music.

A video demo may be found [here][demo].

## Accomplishments

### Complete Rewrite of Hand Tracking Algorithm

Using [this algorithm][handalg] as a base, I rewrote the code to take full
advantage of C++11 features such as range-based for, move semantics, and lambda
functions. I also replaced various classes with their OpenCV equivalents. The
code runs much faster and in about half of the lines.

I also modified a few portions of the UI to make it easier to use. I wait on a
specific key to allow the user to ready their hand before the colors are
sampled. I provided a way for the user to run the program full screen. Users
could also select which webcam to use for input in the case that they had
multiple devices attached to their computer.

The algorithm used is relatively straightforward. First, the user holds their
hand up to the camera to cover a number of green squares. Once they are ready,
they press a key. The program takes a sample of the colors under each square,
and averages the colors to obtain a scalar representing the average. From then
on, the program samples the frames provided by the webcam, looking for colors
that match the average color within a certain threshold and making a black and
white silhouette from that calculation. Next, the silhouette is blurred to
remove noise. Ideally, this silhouette will be an outline of the hand.

The next step is to detect the hand itself. This is accomplished by finding the
contours in the silhouette, and removing all contours that don't match a certain
angle (that of a finger). Once the contours are pruned, it is easy to detect the
fingertips because they are the same. By providing a bounding box and the
fingertips as points, the program can easily detect whether the points intersect
the buttons of the interface.

### Boost and SFML

I used two C++ libraries that I had never used before. Boost provided an
`Optional` type that I found useful for my API, and SFML provided facilities for
playing audio.

### Interface

The interface was drawn using the primitives provided by OpenCV. Moving a
fingertip over a button will activate it, which will then play the sound
associated with the button.

## Difficulties Encountered

The color detection algorithm has its weaknesses. Depending on the lighting, the
thresholds used can be too generous, which leads to far too much of the frame
being detected as a hand. I found that wearing a glove that contrasts with the
background helps the accuracy of the detection.

## Building the Code

ChipDJ uses a CMake build system. You must have Boost, SFML, and OpenCV
installed for the build to succeed.

[demo]: https://www.youtube.com/watch?v=bLs7mDvBo50
[handalg]: http://simena86.github.io/blog/2013/08/12/hand-tracking-and-recognition-with-opencv/
