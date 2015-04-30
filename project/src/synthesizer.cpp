#include "synthesizer.hpp"

#include <iostream>
#include <stdexcept>

Synthesizer::Synthesizer() {}

void loadSound(sf::Sound& sound, sf::SoundBuffer& buffer,
               const std::string& fileName) {
    if (!buffer.loadFromFile(fileName)) {
        throw new std::runtime_error(fileName + " not found");
    }
    sound.setBuffer(buffer);
}

void Synthesizer::loadSounds() {
    loadSound(square1, square1Buffer, "assets/ECS Atari Melody 125 BPM.wav");
    loadSound(square2, square2Buffer, "assets/ECS Fidget Chords 125 BPM.wav");
    loadSound(triangle, triangleBuffer, "assets/C64_tri_with_release_009.wav");
    loadSound(noise, noiseBuffer, "assets/ECS FX 08.wav");
    loadSound(drums, drumsBuffer, "assets/ECS Beats 04 130 BPM.wav");
}

void Synthesizer::playSquare1() {
    square1.play();
}

void Synthesizer::playSquare2() {
    square2.play();
}

void Synthesizer::playTriangle() {
    triangle.play();
}

void Synthesizer::playNoise() {
    noise.play();
}

void Synthesizer::playDrums() {
    drums.play();
}
