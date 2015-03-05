#ifndef MEDIAN_BLUR_HPP

#include <opencv2/core/core.hpp>

/*
 * Performs a median blur convolution on an image.
 */
void gpuMedianBlur(const cv::Mat& input, const cv::Mat& output);

#endif  // MEDIAN_BLUR_HPP
