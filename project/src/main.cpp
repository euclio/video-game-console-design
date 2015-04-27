#include <iostream>

#include <boost/program_options.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "hand.hpp"
#include "handDetection.hpp"
#include "interface.hpp"
#include "median.hpp"
#include "silhouette.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {
    // Set up command line options.
    po::options_description options_description("Allowed options");
    options_description.add_options()
        ("help", "produce help message")
        ("webcam,w", po::value<int>()->default_value(0),
         "set webcam number to use")
        ("nofullscreen", "don't start the program in fullscreen mode");

    po::variables_map options;
    po::store(
            po::parse_command_line(argc, argv, options_description), options);
    po::notify(options);

    if (options.count("help")) {
        options_description.print(std::cout);
        return EXIT_SUCCESS;
    }

    const auto mainWindowName = "display";
    cv::VideoCapture webcam(options.at("webcam").as<int>());
    cv::VideoWriter output;

    cv::namedWindow(mainWindowName, CV_WINDOW_NORMAL);
    if (!options.count("nofullscreen")) {
        cv::setWindowProperty(mainWindowName,
                              CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    }
    cv::namedWindow("silhouette");

    cv::Mat frame, silhouette;
    webcam >> frame;

    auto regionsOfInterest = getRegionsOfInterest(frame);
    auto colorSamples = findAverageColorOfHand(webcam, mainWindowName);

    Interface interface(frame);
    interface.addListener(0, [] {
        std::cout << "Activated" << std::endl;
    });

    do {
        webcam >> frame;

        silhouette = produceSilhouette(frame, colorSamples);
        boost::optional<Hand> hand = detectHand(frame, silhouette);

        if (boost::optional<Hand> hand = detectHand(frame, silhouette)) {
            hand->calculateFingertips();
            hand->drawFingerTips(frame);
            hand->drawContours(frame);

            interface.checkInteractions(*hand);
        }

        interface.drawInterface(frame);

        cv::imshow("silhouette", silhouette);
        cv::imshow(mainWindowName, frame);
    } while((cv::waitKey(10) & 0xFF) != 'q');

    cv::destroyAllWindows();
    output.release();
}
