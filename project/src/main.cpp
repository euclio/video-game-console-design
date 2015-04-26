#include <opencv2/highgui/highgui.hpp>

#include "hand.hpp"
#include "handDetection.hpp"
#include "median.hpp"
#include "silhouette.hpp"

int main() {
    const auto windowName = "median";
    cv::VideoCapture webcam(0);
    cv::VideoWriter output;

    cv::namedWindow("median");
    cv::namedWindow("silhouette");

    cv::Mat frame, silhouette;
    webcam >> frame;

    auto regionsOfInterest = getRegionsOfInterest(frame);
    auto colorSamples = findAverageColorOfHand(webcam, windowName);
    do {
        webcam >> frame;

        silhouette = produceSilhouette(frame, colorSamples);
        boost::optional<Hand> hand = detectHand(frame, silhouette);

        if (boost::optional<Hand> hand = detectHand(frame, silhouette)) {
            hand->calculateFingertips();
            hand->drawFingerTips(frame);
            hand->drawContours(frame);
        }

        cv::imshow("median", frame);
        cv::imshow("silhouette", silhouette);

    } while(cv::waitKey(30) != char('q'));

    cv::destroyAllWindows();
    output.release();
}
