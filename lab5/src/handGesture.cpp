#include "handGesture.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

HandGesture::HandGesture(SocketServer &server) : server(server) {
    frameNumber = 0;
    nrNoFinger = 0;
    fontFace = cv::FONT_HERSHEY_PLAIN;
}

void HandGesture::initVectors() {
    using std::vector;
    hullI = vector<vector<int>> { contours.size() };
    hullP = vector<vector<cv::Point>> { contours.size() };
    defects = vector<vector<cv::Vec4i>> { contours.size() };
}

void HandGesture::analyzeContours() {
    bRect_height = bRect.height;
    bRect_width = bRect.width;
}

void HandGesture::printGestureInfo(cv::Mat src) {
    const int fontFace = cv::FONT_HERSHEY_PLAIN;
    cv::Scalar fColor(245, 200, 200);
    int xpos = src.cols / 1.5;
    int ypos = src.rows / 1.6;
    const float fontSize = 0.7f;
    const int lineChange = 14;

    std::string info = "Figure info:";
    putText(src, info, cv::Point(ypos, xpos), fontFace, fontSize, fColor);
    xpos += lineChange;

    info = std::string("Number of defects: ") + std::to_string(nrOfDefects);
    putText(src, info, cv::Point(ypos, xpos), fontFace, fontSize, fColor);
    xpos += lineChange;

    info = std::string("bounding box height, width ") +
           std::to_string(bRect_height) + std::string(" , ") +
           std::to_string(bRect_width);
    putText(src, info, cv::Point(ypos, xpos), fontFace, fontSize, fColor);
    xpos += lineChange;

    info = std::string("Is hand: ") + std::to_string(isHand);
    putText(src, info, cv::Point(ypos, xpos), fontFace, fontSize, fColor);

    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

    d.AddMember("x", rapidjson::Value(bRect.x), allocator);
    d.AddMember("y", rapidjson::Value(bRect.y), allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    std::string data = buffer.GetString();
    server.write_data(data.c_str(), data.size());
}

bool HandGesture::detectIfHand() {
    analyzeContours();
    double h = bRect_height;
    double w = bRect_width;
    isHand = true;

    if(fingerTips.size() > 5) {
        isHand = false;
    } else if(h == 0 || w == 0) {
        isHand = false;
    } else if(h / w > 4 || w / h > 4) {
        isHand = false;
    } else if(bRect.x < 20) {
        isHand = false;
    }

    return isHand;
}

// remove fingertips that are too close to
// eachother
void HandGesture::removeRedundantFingerTips() {
    std::vector<cv::Point> newFingers;

    for (int i = 0; i < fingerTips.size(); i++) {
        for (int j = i; j < fingerTips.size(); j++) {
            if (cv::norm(fingerTips[i] - fingerTips[j]) < 10 && i != j) {
                // Do nothing
            } else {
                newFingers.push_back(fingerTips[i]);
                break;
            }
        }
    }

    fingerTips.swap(newFingers);
}

void HandGesture::computeFingerNumber() {
    std::sort(fingerNumbers.begin(), fingerNumbers.end());
    int frequentNr;
    int thisNumberFreq = 1;
    int highestFreq = 1;
    frequentNr = fingerNumbers[0];

    for(int i = 1; i < fingerNumbers.size(); i++) {
        if(fingerNumbers[i - 1] != fingerNumbers[i]) {
            if(thisNumberFreq > highestFreq) {
                frequentNr = fingerNumbers[i - 1];
                highestFreq = thisNumberFreq;
            }

            thisNumberFreq = 0;
        }

        thisNumberFreq++;
    }

    if(thisNumberFreq > highestFreq) {
        frequentNr = fingerNumbers[fingerNumbers.size() - 1];
    }

    mostFrequentFingerNumber = frequentNr;
}

void HandGesture::addFingerNumberToVector() {
    int i = fingerTips.size();
    fingerNumbers.push_back(i);
}

// add the calculated number of fingers to image m->src
void HandGesture::addNumberToImg(MyImage *m) {
    int xPos = 10;
    int yPos = 10;
    int offset = 30;
    float fontSize = 1.5f;
    int fontFace = cv::FONT_HERSHEY_PLAIN;

    for(int i = 0; i < numbers2Display.size(); i++) {
        rectangle(m->src, cv::Point(xPos, yPos), cv::Point(xPos + offset, yPos + offset), numberColor, 2);
        putText(m->src, std::to_string(numbers2Display[i]),
                cv::Point(xPos + 7, yPos + offset - 3), fontFace, fontSize,
                numberColor);
        xPos += 40;

        if(xPos > (m->src.cols - m->src.cols / 3.2)) {
            yPos += 40;
            xPos = 10;
        }
    }
}

// calculate most frequent numbers of fingers
// over 20 frames
void HandGesture::getFingerNumber(MyImage *m) {
    removeRedundantFingerTips();

    if(bRect.height > m->src.rows / 2 && nrNoFinger > 12 && isHand) {
        numberColor = cv::Scalar(0, 200, 0);
        addFingerNumberToVector();

        if(frameNumber > 12) {
            nrNoFinger = 0;
            frameNumber = 0;
            computeFingerNumber();
            numbers2Display.push_back(mostFrequentFingerNumber);
            fingerNumbers.clear();
        } else {
            frameNumber++;
        }
    } else {
        nrNoFinger++;
        numberColor = cv::Scalar(200, 200, 200);
    }

    addNumberToImg(m);
}

float HandGesture::getAngle(cv::Point s, cv::Point f, cv::Point e) {
    float l1 = cv::norm(f - s);
    float l2 = cv::norm(f - e);
    float dot = (s.x - f.x) * (e.x - f.x) + (s.y - f.y) * (e.y - f.y);
    float angle = acos(dot / (l1 * l2));
    angle = angle * 180 / M_PI;
    return angle;
}

void HandGesture::eleminateDefects(MyImage *m) {
    int tolerance =  bRect_height / 5;
    float angleTol = 95;
    std::vector<cv::Vec4i> newDefects;
    int startidx, endidx, faridx;
    std::vector<cv::Vec4i>::iterator d = defects[cIdx].begin();

    while(d != defects[cIdx].end()) {
        cv::Vec4i& v = (*d);
        startidx = v[0];
        cv::Point ptStart(contours[cIdx][startidx]);
        endidx = v[1];
        cv::Point ptEnd(contours[cIdx][endidx]);
        faridx = v[2];
        cv::Point ptFar(contours[cIdx][faridx]);

        if (cv::norm(ptStart - ptFar) > tolerance
                && cv::norm(ptEnd - ptFar) > tolerance
                && getAngle(ptStart, ptFar, ptEnd) < angleTol) {
            if (ptEnd.y > (bRect.y + bRect.height - bRect.height / 4)) {
                // Do nothing
            } else if(ptStart.y > (bRect.y + bRect.height - bRect.height / 4)) {
                // Do nothing
            } else {
                newDefects.push_back(v);
            }
        }

        d++;
    }

    nrOfDefects = newDefects.size();
    defects[cIdx].swap(newDefects);
    removeRedundantEndPoints(defects[cIdx], m);
}

// remove endpoint of convexity defects if they are at the same fingertip
void HandGesture::removeRedundantEndPoints(std::vector<cv::Vec4i> newDefects, MyImage *m) {
    cv::Vec4i temp;
    float avgX, avgY;
    float tolerance = bRect_width / 6;
    int startidx, endidx, faridx;
    int startidx2, endidx2;

    for(int i = 0; i < newDefects.size(); i++) {
        for(int j = i; j < newDefects.size(); j++) {
            startidx = newDefects[i][0];
            cv::Point ptStart(contours[cIdx][startidx]);
            endidx = newDefects[i][1];
            cv::Point ptEnd(contours[cIdx][endidx]);
            startidx2 = newDefects[j][0];
            cv::Point ptStart2(contours[cIdx][startidx2]);
            endidx2 = newDefects[j][1];
            cv::Point ptEnd2(contours[cIdx][endidx2]);

            if (cv::norm(ptStart - ptEnd2) < tolerance) {
                contours[cIdx][startidx] = ptEnd2;
                break;
            }

            if (cv::norm(ptEnd - ptStart2) < tolerance) {
                contours[cIdx][startidx2] = ptEnd;
            }
        }
    }
}

// convexity defects does not check for one finger
// so another method has to check when there are no
// convexity defects
void HandGesture::checkForOneFinger(MyImage *m) {
    int yTol = bRect.height / 6;
    cv::Point highestP;
    highestP.y = m->src.rows;

    for(auto& d : contours[cIdx]) {
        cv::Point v = d;

        if(v.y < highestP.y) {
            highestP = v;
            std::cout << highestP.y << std::endl;
        }
    }

    int n = 0;

    for(auto& d : hullP[cIdx]) {
        cv::Point v = d;
        std::cout << "x " << v.x
                  << " y " <<  v.y
                  << " highestpY " << highestP.y
                  << "ytol " << yTol << std::endl;

        if(v.y < highestP.y + yTol && v.y != highestP.y && v.x != highestP.x) {
            n++;
        }
    }

    if(n == 0) {
        fingerTips.push_back(highestP);
    }
}

void HandGesture::drawFingerTips(MyImage *m) {
    cv::Point p;
    int k = 0;

    for(int i = 0; i < fingerTips.size(); i++) {
        p = fingerTips[i];
        putText(m->src, std::to_string(i), p - cv::Point(0, 30),
                fontFace, 1.2f, cv::Scalar(200, 200, 200), 2);
        circle(m->src, p, 5, cv::Scalar(100, 255, 100), 4);
    }
}

void HandGesture::getFingerTips(MyImage *m) {
    fingerTips.clear();
    int i = 0;

    for(auto& d : defects[cIdx]) {
        cv::Vec4i& v = d;
        int startidx = v[0];
        cv::Point ptStart(contours[cIdx][startidx]);
        int endidx = v[1];
        cv::Point ptEnd(contours[cIdx][endidx]);
        int faridx = v[2];
        cv::Point ptFar(contours[cIdx][faridx]);

        if(i == 0) {
            fingerTips.push_back(ptStart);
            i++;
        }

        fingerTips.push_back(ptEnd);
        i++;
    }

    if(fingerTips.size() == 0) {
        checkForOneFinger(m);
    }
}
