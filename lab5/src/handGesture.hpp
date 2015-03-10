#ifndef HAND_GESTURE_HPP
#define HAND_GESTURE_HPP

#include <vector>
#include <string>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "myImage.hpp"
#include "socketServer.hpp"

class HandGesture{
    public:
        MyImage m;
        HandGesture(SocketServer &server);
        std::vector<std::vector<cv::Point>> contours;
        std::vector<std::vector<int>> hullI;
        std::vector<std::vector<cv::Point>> hullP;
        std::vector<std::vector<cv::Vec4i>> defects;
        std::vector<cv::Point> fingerTips;
        cv::Rect rect;
        void printGestureInfo(cv::Mat src);
        int cIdx;
        int frameNumber;
        int mostFrequentFingerNumber;
        int nrOfDefects;
        cv::Rect bRect;
        double bRect_width;
        double bRect_height;
        bool isHand;
        bool detectIfHand();
        void initVectors();
        void getFingerNumber(MyImage *m);
        void eleminateDefects(MyImage *m);
        void getFingerTips(MyImage *m);
        void drawFingerTips(MyImage *m);
    private:
        SocketServer server;
        std::string bool2string(bool tf);
        int fontFace;
        int prevNrFingerTips;
        void checkForOneFinger(MyImage *m);
        float getAngle(cv::Point s, cv::Point f, cv::Point e);
        std::vector<int> fingerNumbers;
        void analyzeContours();
        std::string intToString(int number);
        void computeFingerNumber();
        void drawNewNumber(MyImage *m);
        void addNumberToImg(MyImage *m);
        std::vector<int> numbers2Display;
        void addFingerNumberToVector();
        cv::Scalar numberColor;
        int nrNoFinger;
        float distanceP2P(cv::Point a, cv::Point b);
        void removeRedundantEndPoints(std::vector<cv::Vec4i> newDefects, MyImage *m);
        void removeRedundantFingerTips();
};

#endif // HAND_GESTURE_HPP
