#include "silhouette.hpp"

#include <vector>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

cv::Mat produceSilhouette(const cv::Mat& frame,
                          const std::vector<cv::Scalar>& colors) {
    std::vector<cv::Mat> silhouetteSamples;
    cv::Mat lowResFrame, silhouetteSum;

    cv::pyrDown(frame, lowResFrame);
    cv::blur(lowResFrame, lowResFrame, cv::Size(3, 3));
    cv::cvtColor(lowResFrame, lowResFrame, cv::COLOR_BGR2HLS);

    for (auto& color : colors) {
        const cv::Scalar lowerBound = color - cv::Scalar(12, 30, 80);
        const cv::Scalar upperBound = color + cv::Scalar(7, 40, 80);

        cv::Mat silhouette(lowResFrame.rows, lowResFrame.cols, CV_8UC1);
        cv::inRange(lowResFrame, lowerBound, upperBound, silhouette);

        silhouetteSamples.push_back(silhouette);
    }

    silhouetteSamples[0].copyTo(silhouetteSum);
    for (size_t i = 1; i < silhouetteSamples.size(); ++i) {
        silhouetteSum += silhouetteSamples[i];
    }

    cv::medianBlur(silhouetteSum, silhouetteSum, 7);

    return silhouetteSum;
}
