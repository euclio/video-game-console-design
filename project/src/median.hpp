#ifndef MEDIAN_HPP
#define MEDIAN_HPP

#include <opencv2/highgui/highgui.hpp>

cv::Scalar findAverageColorOfHand(cv::VideoCapture& input,
                                  const std::string& windowName);

#endif  // MEDIAN_HPP
