#ifndef SYNTHESIZER_HPP
#define SYNTHESIZER_HPP

#include <SFML/Audio.hpp>

class Synthesizer {
    public:
        Synthesizer();
        void loadSounds();
        void playSquare1();
        void playSquare2();
        void playTriangle();
        void playNoise();
        void playDrums();
    private:
        sf::Sound square1, square2, triangle, noise, drums;
        sf::SoundBuffer square1Buffer, square2Buffer, triangleBuffer,
                        noiseBuffer, drumsBuffer;
};

#endif
