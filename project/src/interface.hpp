#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <opencv2/core/core.hpp>

#include "button.hpp"

class Interface {
    public:
        Interface(const cv::Mat& frame);
        void drawInterface(cv::Mat& source);
    private:
        std::vector<Button> buttons;
};

#endif
