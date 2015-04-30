#include <iostream>

#include <boost/program_options.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "hand.hpp"
#include "handDetection.hpp"
#include "interface.hpp"
#include "median.hpp"
#include "silhouette.hpp"
#include "synthesizer.hpp"

namespace po = boost::program_options;

const auto HORIZONTAL_FLIP = 1;

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
    cv::flip(frame, frame, HORIZONTAL_FLIP);

    auto regionsOfInterest = getRegionsOfInterest(frame);
    auto colorSamples = findAverageColorOfHand(webcam, mainWindowName);

    Synthesizer synth;
    synth.loadSounds();

    Interface interface(frame);
    interface.addListener(0, [&synth] { synth.playSquare1(); });
    interface.addListener(1, [&synth] { synth.playSquare2(); });
    interface.addListener(2, [&synth] { synth.playTriangle(); });
    interface.addListener(3, [&synth] { synth.playNoise(); });
    interface.addListener(4, [&synth] { synth.playDrums(); });

    do {
        webcam >> frame;

        cv::flip(frame, frame, HORIZONTAL_FLIP);

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
