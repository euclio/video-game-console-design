#include <vector>

#include <opencv2/core/core.hpp>

#include "median.hpp"

std::vector<cv::Rect> getRegionsOfInterest(const cv::Mat& frame) {
    const cv::Size rectSize(20, 20);

    std::vector<cv::Rect> regions;

    cv::Point topLeft(frame.cols / 3, frame.rows / 6);

    regions.emplace_back(topLeft, rectSize);

    return regions;
}

void drawRectangle(cv::Mat& image, cv::Rect& rectangle) {
    const int borderThickness = 2;
    const cv::Scalar color(0, 255, 0);

    cv::rectangle(image, rectangle, color, borderThickness);
}

void waitForPalmCover(cv::VideoCapture& input) {
    cv::Mat frame;

    input >> frame;

    auto regionsOfInterest = getRegionsOfInterest(frame);

    for (;;) {
        input >> frame;

        for (auto& roi : regionsOfInterest) {
            drawRectangle(frame, roi);
        }

        cv::imshow("median", frame);
        cv::waitKey(10);
    }
}
