#include "hand.hpp"

#include <opencv2/imgproc/imgproc.hpp>

Hand::Hand(const cv::Mat& source,
           const cv::Rect& boundingBox,
           const std::vector<cv::Point>& contours,
           const std::vector<cv::Vec4i>& defects,
           const std::vector<std::vector<cv::Point>>& hull,
           const size_t biggestContourIndex) :
    isHandDetected(false), fingertips(), boundingBox(boundingBox),
    contours(contours), defects(defects), hull(hull), source(source),
    biggestContourIndex(biggestContourIndex) {}

bool Hand::isHand() {
    return isHandDetected;
}

void Hand::detectIfHand() {
    isHandDetected = true;

    if (fingertips.size() > 5) {
        isHandDetected = false;
    } else if (boundingBox.height == 0 || boundingBox.width == 0) {
        isHandDetected = false;
    } else if (boundingBox.height / boundingBox.width > 4 ||
               boundingBox.width / boundingBox.height > 4) {
        isHandDetected = false;
    } else if (boundingBox.x < 20) {
        isHandDetected = false;
    }
}

void Hand::calculateFingertips() {
    fingertips.clear();
    size_t i = 0;

    for (auto& defect : defects) {
        size_t startIdx = defect[0];
        cv::Point startPt(contours[startIdx]);
        size_t endIdx = defect[1];
        cv::Point endPt(contours[endIdx]);
        size_t farIdx = defect[2];
        cv::Point farPt(contours[farIdx]);

        if (i == 0) {
            fingertips.push_back(startPt);
            ++i;
        }

        fingertips.push_back(endPt);
        ++i;
    }

    if (fingertips.size() == 0) {
        checkForOneFinger();
    }
}

void Hand::checkForOneFinger() {
    int yTol = boundingBox.height / 6;
    cv::Point highestPoint;
    highestPoint.y = source.rows;

    for (auto& contour : contours) {
        if (contour.y < highestPoint.y) {
            highestPoint = contour;
        }
    }

    int n = 0;

    for (auto& hullPoint : hull[biggestContourIndex]) {
        if (hullPoint.y < highestPoint.y + yTol
                && hullPoint.y != highestPoint.y
                && hullPoint.x != highestPoint.x) {
            ++n;
        }
    }

    if (n == 0) {
        fingertips.push_back(highestPoint);
    }
}

void Hand::drawFingerTips(cv::Mat& source) {
    cv::Point p;
    const auto fontFace = cv::FONT_HERSHEY_PLAIN;

    for (size_t i = 0; i < fingertips.size(); ++i) {
        p = fingertips[i];
        cv::putText(source, std::to_string(i), p - cv::Point(0, 30),
                    fontFace, 1.2f, cv::Scalar(100, 255, 100), 4);
        cv::circle(source, p, 5, cv::Scalar(100, 255, 100, 4));
    }
}

void Hand::drawContours(cv::Mat& source) {
    cv::drawContours(source, hull, biggestContourIndex,
                     cv::Scalar(0, 0, 250), 10, 8, std::vector<cv::Vec4i>(),
                     0, cv::Point());
    cv::rectangle(source, boundingBox.tl(), boundingBox.br(),
                  cv::Scalar(0, 0, 200));
}

std::vector<cv::Point> Hand::getFingertips() const {
    return fingertips;
}
