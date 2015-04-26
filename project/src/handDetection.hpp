#ifndef HAND_DETECTION_HPP
#define HAND_DETECTION_HPP

#include <boost/optional.hpp>
#include <opencv2/core/core.hpp>

#include "hand.hpp"

boost::optional<Hand> detectHand(cv::Mat& source, const cv::Mat& silhouette);

#endif
