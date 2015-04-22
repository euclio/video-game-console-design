#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "median.hpp"

const auto numSamples = 50;
const auto frameDelay = 30;

void printText(cv::Mat &img, const cv::Point& origin, const std::string& text) {
    const auto fontFace = cv::FONT_HERSHEY_PLAIN;
    const auto scaleFactor = 1.2f;
    const auto color = cv::Scalar(200, 0, 0);
    const auto thickness = 2;

    cv::putText(img, text, origin, fontFace, scaleFactor, color, thickness);
}

std::vector<cv::Rect> getRegionsOfInterest(const cv::Mat& frame) {
    const cv::Size rectSize(20, 20);

    std::vector<cv::Rect> regions;

    std::vector<cv::Point2d> rectangleOrigins = {
        { frame.cols / 4.0, frame.rows / 2.0 },
        { frame.cols / 3.0, frame.rows / 6.0 },
        { frame.cols / 3.0, frame.rows / 1.5 },
        { frame.cols / 2.5, frame.rows / 2.5 },
        { frame.cols / 2.0, frame.rows / 2.0 },
        { frame.cols / 2.0, frame.rows / 1.8 },
    };

    for (auto& origin : rectangleOrigins) {
        regions.emplace_back(origin, rectSize);
    }

    return regions;
}

void drawRectangle(cv::Mat& image, const cv::Rect& rectangle) {
    const int borderThickness = 2;
    const cv::Scalar color(0, 255, 0);

    cv::rectangle(image, rectangle, color, borderThickness);
}

void waitForPalmCover(cv::VideoCapture& input, const std::string& windowName) {
    cv::Mat frame;

    input >> frame;

    auto regionsOfInterest = getRegionsOfInterest(frame);

    do {
        input >> frame;

        for (auto& roi : regionsOfInterest) {
            drawRectangle(frame, roi);
        }

        printText(frame, { frame.cols / 2, frame.rows / 10 },
                  "Cover rectangles with palm.");
        printText(frame, { frame.cols / 2, frame.rows / 3 },
                  "Press any key when ready.");
        cv::imshow(windowName, frame);
    } while(cv::waitKey(frameDelay) == -1);
}

cv::Scalar findAverageColor(const cv::Mat& frame,
                            const cv::Rect& regionOfInterest) {
    std::vector<cv::Mat> channels;
    cv::Mat roi = frame(regionOfInterest);

    return cv::mean(roi);
}

cv::Scalar getMedian(const std::vector<cv::Scalar>& values) {
    cv::Scalar medians;

    for (size_t channel = 0; channel < 4; ++channel) {
        std::vector<double> channelValues;
        for (auto& scalar : values) {
            channelValues.push_back(scalar[channel]);
        }

        std::sort(channelValues.begin(), channelValues.end());
        double median;
        if (channelValues.size() % 2 == 0) {
            median = channelValues[channelValues.size() / 2 - 1];
        } else {
            median = channelValues[channelValues.size() / 2];
        }
        medians[channel] = median;
    }

    return medians;
}

std::vector<cv::Scalar> findAverageColorOfHand(cv::VideoCapture& input,
                                               const std::string& windowName) {
    waitForPalmCover(input, windowName);

    std::vector<std::vector<int>> channelSamples;
    cv::Mat frame, hlsFrame;

    input >> frame;
    cv::cvtColor(frame, hlsFrame, cv::COLOR_BGR2HLS);
    auto regionsOfInterest = getRegionsOfInterest(frame);
    std::vector<cv::Scalar> averages;
    for (auto& roi : regionsOfInterest) {
        auto average = findAverageColor(hlsFrame, roi);
        averages.push_back(average);
    }
    printText(frame, { frame.cols / 2, frame.rows / 10},
              "Obtaining averages color...");
    cv::imshow(windowName, frame);
    cv::waitKey(30);

    return averages;
}
