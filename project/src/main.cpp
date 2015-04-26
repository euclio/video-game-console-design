#include <opencv2/highgui/highgui.hpp>

#include <iostream>

#include "hand.hpp"
#include "handDetection.hpp"
#include "median.hpp"
#include "silhouette.hpp"

int main() {
    const auto mainWindowName = "display";
    cv::VideoCapture webcam(0);
    cv::VideoWriter output;

    cv::namedWindow(mainWindowName, CV_WINDOW_NORMAL);
    cv::setWindowProperty(mainWindowName,
                           CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    cv::namedWindow("silhouette");

    cv::Mat frame, silhouette;
    webcam >> frame;

    auto regionsOfInterest = getRegionsOfInterest(frame);
    auto colorSamples = findAverageColorOfHand(webcam, mainWindowName);
    do {
        webcam >> frame;

        silhouette = produceSilhouette(frame, colorSamples);
        boost::optional<Hand> hand = detectHand(frame, silhouette);

        if (boost::optional<Hand> hand = detectHand(frame, silhouette)) {
            hand->calculateFingertips();
            hand->drawFingerTips(frame);
            hand->drawContours(frame);
        }

        cv::imshow("silhouette", silhouette);
        cv::imshow(mainWindowName, frame);
    } while((cv::waitKey(30) & 0xFF) != 'q');

    cv::destroyAllWindows();
    output.release();
}
