#ifndef HAND_HPP
#define HAND_HPP

#include <vector>
#include <string>

#include <opencv2/core/core.hpp>

class Hand {
    public:
        Hand(const cv::Mat& source,
             const cv::Rect& boundingBox,
             const std::vector<cv::Point>& contours,
             const std::vector<cv::Vec4i>& defects,
             const std::vector<cv::Point>& hull);
        void detectIfHand();
        void calculateFingertips();
        bool isHand();
        void drawFingerTips(cv::Mat& source);
        void drawContours(cv::Mat& source,
                          const std::vector<std::vector<cv::Point>>& hull,
                          size_t biggestContourIndex);
    private:
        bool isHandDetected;
        cv::Mat source;
        std::vector<cv::Point> fingerTips;
        std::vector<cv::Point> contours;
        std::vector<cv::Vec4i> defects;
        std::vector<cv::Point> hull;
        cv::Rect boundingBox;
        void checkForOneFinger();
};

#endif
