#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <functional>

#include <opencv2/core/core.hpp>

#include "button.hpp"
#include "hand.hpp"

class Interface {
    public:
        Interface(const cv::Mat& frame);
        void drawInterface(cv::Mat& source);
        void checkInteractions(const Hand& hand);
        std::vector<Button> getButtons() const;
        void addListener(size_t button, std::function<void()> listener);
    private:
        std::vector<Button> buttons;
};

#endif
