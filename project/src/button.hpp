#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <chrono>

#include <opencv2/core/core.hpp>

class Button {
    public:
        Button(const cv::Rect& boundingBox, const std::string& text);
        void activate();
        void deactivate();
        bool isActivated();
        void drawOn(cv::Mat& frame);
    private:
        cv::Rect boundingBox;
        bool activated;
        std::chrono::system_clock::time_point lastActivated;
        std::string text;
};

#endif
