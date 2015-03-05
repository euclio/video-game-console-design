#ifndef MYIMAGE_HPP
#define MYIMAGE_HPP

#include <vector>

#include <opencv2/gpu/gpu.hpp>
#include <opencv2/opencv.hpp>

class MyImage {
public:
    MyImage(int webCamera);
    MyImage();
    cv::Mat srcLR;
    cv::Mat src;
    cv::Mat bw;
    std::vector<cv::Mat> bwList;
    cv::VideoCapture cap;
    int cameraSrc;
    void initWebCamera(int i);
};

#endif // MY_IMAGE_HPP
