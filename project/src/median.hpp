#ifndef MEDIAN_HPP
#define MEDIAN_HPP

#include <vector>

#include <opencv2/highgui/highgui.hpp>

std::vector<cv::Scalar> findAverageColorOfHand(cv::VideoCapture& input,
                                               const std::string& windowName);

std::vector<cv::Rect> getRegionsOfInterest(const cv::Mat& frame);

#endif  // MEDIAN_HPP
