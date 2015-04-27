#include "interface.hpp"

Interface::Interface(const cv::Mat& frame) {
    cv::Rect boundingBox(20, 20, 100, 60);
    buttons.emplace_back(boundingBox, "Track 1");
}

void Interface::drawInterface(cv::Mat& source) {
    for (auto& button : buttons) {
        button.drawOn(source);
    }
}
