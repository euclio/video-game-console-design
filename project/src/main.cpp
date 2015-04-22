#include <opencv2/highgui/highgui.hpp>

#include "median.hpp"

int main() {
    const auto windowName = "median";
    cv::VideoCapture webcam(0);
    cv::VideoWriter output;

    cv::namedWindow("median");

    auto color = findAverageColorOfHand(webcam, windowName);
}
