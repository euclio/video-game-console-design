#include "handDetection.hpp"

#include "hand.hpp"

#include <cmath>

#include <opencv2/imgproc/imgproc.hpp>

template<class T> using hull = std::vector<std::vector<T>>;

float getAngle(cv::Point s, cv::Point f, cv::Point e) {
    float l1 = cv::norm(f - s);
    float l2 = cv::norm(f - e);
    float dot = (s.x - f.x) * (e.x - f.x) + (s.y - f.y) * (e.y - f.y);
    float angle = std::acos(dot / (l1 * l2));
    angle = angle * 180 / M_PI;
    return angle;
}

void removeRedundantEndpoints(const std::vector<cv::Vec4i>& newDefects,
                              std::vector<cv::Point>& biggestContour,
                              const cv::Rect& boundingRect) {
    float avgX, avgY;
    const float tolerance = boundingRect.width / 6;

    for (size_t i = 0; i < newDefects.size(); i++) {
        for (size_t j = 0; j < newDefects.size(); j++) {
            size_t startIdx = newDefects[i][0];
            size_t startIdx2 = newDefects[j][0];

            cv::Point startPt(biggestContour[startIdx]);
            cv::Point endPt(biggestContour[newDefects[i][1]]);
            cv::Point startPt2(biggestContour[startIdx2]);
            cv::Point endPt2(biggestContour[newDefects[j][1]]);

            if (cv::norm(startPt - endPt2) < tolerance) {
                biggestContour[startIdx] = endPt2;
                break;
            }

            if (cv::norm(endPt - startPt2) < tolerance) {
                biggestContour[startIdx2] = endPt;
            }
        }
    }
}

void eliminateDefects(std::vector<cv::Point>& biggestContour,
                      std::vector<cv::Vec4i>& biggestDefects,
                      const cv::Rect& boundingRect) {
    const int tolerance = boundingRect.height / 5;
    const float angleTolerance = 95;
    std::vector<cv::Vec4i> newDefects;

    for (auto& defect : biggestDefects) {
        cv::Point ptStart = biggestContour[defect[0]];
        cv::Point ptEnd = biggestContour[defect[1]];
        cv::Point ptFar = biggestContour[defect[2]];

        if (cv::norm(ptStart - ptFar) > tolerance
                && cv::norm(ptEnd - ptFar) > tolerance
                && getAngle(ptStart, ptFar, ptEnd) < angleTolerance) {
            bool isPtEndOutsideBoundingBox =
                ptEnd.y > (boundingRect.y + boundingRect.height -
                           boundingRect.height / 4);
            bool isPtStartOutsideBoundBox =
                ptStart.y > (boundingRect.y + boundingRect.height -
                             boundingRect.height / 4);
            if (!isPtEndOutsideBoundingBox && !isPtStartOutsideBoundBox) {
                newDefects.push_back(defect);
            }
        }
    }

    biggestDefects = newDefects;
    removeRedundantEndpoints(newDefects, biggestContour, boundingRect);
}

boost::optional<Hand> detectHand(cv::Mat& source, const cv::Mat& silhouette) {
    boost::optional<Hand> hand;
    cv::Mat silhouetteCopy;
    cv::pyrUp(silhouette, silhouetteCopy);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(silhouetteCopy, contours,
                     CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    auto biggestContour =
        std::max_element(contours.begin(), contours.end(),
            [](std::vector<cv::Point> a, std::vector<cv::Point> b) {
                return a.size() < b.size();
            });
    if (biggestContour == contours.end()) {
        return boost::none;
    }

    auto biggestContourIdx = std::distance(contours.begin(), biggestContour);
    auto boundingRect = cv::boundingRect(cv::Mat(*biggestContour));

    const bool clockwise = false;
    hull<cv::Point> pointHull(contours.size());
    cv::convexHull(
            cv::Mat(*biggestContour), pointHull[biggestContourIdx], clockwise,
            /* return_points */ true);
    hull<int> intHull(contours.size());
    cv::convexHull(
            cv::Mat(*biggestContour), intHull[biggestContourIdx], clockwise,
            /* return_points */ false);
    const auto epsilon = 18;
    cv::approxPolyDP(
            cv::Mat(pointHull[biggestContourIdx]),
            pointHull[biggestContourIdx], epsilon, /* closed */ true);

    if (biggestContour->size() > 3) {
        std::vector<std::vector<cv::Vec4i>> defects(contours.size());
        cv::convexityDefects(contours[biggestContourIdx],
                             intHull[biggestContourIdx],
                             defects[biggestContourIdx]);
        eliminateDefects(
                *biggestContour, defects[biggestContourIdx], boundingRect);

        Hand hand(source, boundingRect, *biggestContour,
                  defects[biggestContourIdx], pointHull, biggestContourIdx);
        hand.detectIfHand();
        return hand;
    }

    return boost::none;
}
