#include <opencv2/highgui/highgui.hpp>

#include "median.hpp"

int main() {
    cv::VideoCapture webcam(0);
    cv::VideoWriter output;

    cv::namedWindow("median");

    waitForPalmCover(webcam);
}
