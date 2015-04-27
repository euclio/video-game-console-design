#include "synthesizer.hpp"

#include <iostream>

Synthesizer::Synthesizer() {}

void Synthesizer::playSquare1() {
    std::cout << "Square 1 activated" << std::endl;
}

void Synthesizer::playSquare2() {
    std::cout << "Square 2 activated" << std::endl;
}

void Synthesizer::playTriangle() {
    std::cout << "Triangle activated" << std::endl;
}

void Synthesizer::playNoise() {
    std::cout << "Noise activated" << std::endl;
}
