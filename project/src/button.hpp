#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <chrono>
#include <functional>

#include <opencv2/core/core.hpp>

class Button {
    public:
        Button(const cv::Rect& boundingBox, const std::string& text);
        void activate();
        void deactivate();
        bool isActivated();
        void drawOn(cv::Mat& frame);
        cv::Rect getBoundingBox() const;
        void addListener(std::function<void()> listener);
        void notifyListeners();
    private:
        cv::Rect boundingBox;
        bool activated;
        std::chrono::system_clock::time_point lastActivated;
        std::string text;
        std::vector<std::function<void()>> listeners;
};

#endif
