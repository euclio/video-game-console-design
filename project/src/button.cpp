#include "button.hpp"

#include <chrono>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>

using namespace std::chrono_literals;

const auto activationTime = 300ms;
const cv::Scalar red(0, 0, 200);
const cv::Scalar blue(200, 0, 0);
const auto font = CV_FONT_HERSHEY_PLAIN;

Button::Button(const cv::Rect& boundingBox, const std::string& text) :
    boundingBox(boundingBox), text(text), longActivated(false),
    activated(false) {}

void Button::press() {
    activated = true;
}

void Button::release() {
    activated = false;
}

bool Button::isActivated() {
    return activated;
}

bool Button::isLongActivated() {
    return longActivated;
}

void Button::drawOn(cv::Mat& frame) {
    const auto fontScale = 1.0;

    auto thickness = isActivated() ? CV_FILLED : 1;
    cv::rectangle(frame, boundingBox.tl(), boundingBox.br(), blue, thickness);
    if (isLongActivated()) {
        cv::rectangle(frame, boundingBox.tl(), boundingBox.br(), red, 3);
    }

    cv::putText(frame, text, boundingBox.tl() + cv::Point(10, 20), font,
                fontScale, red);
}

cv::Rect Button::getBoundingBox() const {
    return boundingBox;
}

void Button::addListener(std::function<void()> listener) {
    listeners.push_back(listener);
}

void Button::notifyListeners() {
    for (auto& listener : listeners) {
        listener();
    }
}
