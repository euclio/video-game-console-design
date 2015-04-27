#include "button.hpp"

#include <opencv2/imgproc/imgproc.hpp>

const int activationTime = 500;
const cv::Scalar red(200, 0, 0);
const auto font = CV_FONT_HERSHEY_PLAIN;

Button::Button(const cv::Rect& boundingBox, const std::string& text) :
    boundingBox(boundingBox), text(text) {}

void Button::activate() {
    if (!activated) {
        lastActivated = std::chrono::system_clock::now();
    }

    activated = true;
}

void Button::deactivate() {
    activated = false;
}

bool Button::isActivated() {
    return activated;
}

void Button::drawOn(cv::Mat& frame) {
    const auto fontScale = 1.0;
    cv::rectangle(frame, boundingBox.tl(), boundingBox.br(), red);
    cv::putText(frame, text, boundingBox.tl() + cv::Point(10, 20), font,
                fontScale, red);
}
