#ifndef CONTOURS_HPP
#define CONTOURS_HPP

#include <opencv2/core/core.hpp>

#include "hand.hpp"

cv::Mat findContours(cv::Mat& source, const cv::Mat& silhouette);

#endif
