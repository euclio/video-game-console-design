#include "interface.hpp"

Interface::Interface(const cv::Mat& frame) {
    buttons.emplace_back(cv::Rect(20, 20, 100, 60),  "Square 1");
    buttons.emplace_back(cv::Rect(140, 20, 100, 60), "Square 2");
    buttons.emplace_back(cv::Rect(260, 20, 100, 60), "Triangle");
    buttons.emplace_back(cv::Rect(380, 20, 100, 60), "Noise");
    buttons.emplace_back(cv::Rect(500, 20, 100, 60), "Drums");
}

void Interface::drawInterface(cv::Mat& source) {
    for (auto& button : buttons) {
        button.drawOn(source);
    }
}

void Interface::checkInteractions(const Hand& hand) {
    for (auto& fingertip : hand.getFingertips()) {
        for (auto& button : buttons) {
            if (button.getBoundingBox().contains(fingertip)) {
                button.press();
            } else  {
                button.release();
            }

            if (button.isActivated()) {
                button.notifyListeners();
            }
        }
    }
}

void Interface::addListener(size_t button, std::function<void()> listener) {
    buttons[button].addListener(listener);
}
