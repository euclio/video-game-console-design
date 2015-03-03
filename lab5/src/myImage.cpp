#include "myImage.hpp"

MyImage::MyImage() {}

MyImage::MyImage(int webCamera) {
    cameraSrc = webCamera;
    cap = cv::VideoCapture(webCamera);
}

