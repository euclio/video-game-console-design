#ifndef SILHOUETTE_HPP
#define SILHOUETTE_HPP

#include <opencv2/core/core.hpp>

cv::Mat produceSilhouette(const cv::Mat& frame,
                          const std::vector<cv::Scalar>& color);

#endif
