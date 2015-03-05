#include <cmath>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "opencv2/gpu/gpu.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include <opencv2/opencv.hpp>

#include "handGesture.hpp"
#include "medianBlur.hpp"
#include "myImage.hpp"
#include "roi.hpp"

const size_t NSAMPLES = 7;
const int fontFace = cv::FONT_HERSHEY_PLAIN;
const int ORIGCOL2COL = CV_BGR2HLS;
const int COL2ORIGCOL = CV_HLS2BGR;

/* Global Variables  */
int square_len;
int avgColor[NSAMPLES][3] ;
int c_lower[NSAMPLES][3];
int c_upper[NSAMPLES][3];
int avgBGR[3];
int nrOfDefects;
int iSinceKFInit;
struct dim {int w; int h;} boundingDim;
cv::VideoWriter out;
cv::Mat edges;
My_ROI roi1, roi2, roi3, roi4, roi5, roi6;
std::vector<My_ROI> roi;
std::vector<cv::KalmanFilter> kf;
cv::vector<cv::Mat_<float>> measurement;

/* end global variables */

void init(MyImage *m) {
    square_len = 20;
    iSinceKFInit = 0;
}

// change a color from one space to another
void col2origCol(int hsv[3], int bgr[3], cv::Mat src) {
    cv::Mat avgBGRMat = src.clone();

    for(int i = 0; i < 3; i++) {
        avgBGRMat.data[i] = hsv[i];
    }

    cvtColor(avgBGRMat, avgBGRMat, COL2ORIGCOL);

    for(int i = 0; i < 3; i++) {
        bgr[i] = avgBGRMat.data[i];
    }
}

void printText(cv::Mat src, std::string text) {
    putText(src, text, cv::Point(src.cols / 2, src.rows / 10),
            fontFace, 1.2f, cv::Scalar(200, 0, 0), 2);
}

void waitForPalmCover(MyImage *m) {
    m->cap >> m->src;
    flip(m->src, m->src, 1);
    roi.push_back(
            My_ROI(cv::Point(m->src.cols / 3, m->src.rows / 6),
                   cv::Point(m->src.cols / 3 + square_len, m->src.rows / 6 + square_len),
                   m->src));
    roi.push_back(
            My_ROI(cv::Point(m->src.cols / 4, m->src.rows / 2),
                   cv::Point(m->src.cols / 4 + square_len, m->src.rows / 2 + square_len),
                   m->src));
    roi.push_back(
            My_ROI(cv::Point(m->src.cols / 3, m->src.rows / 1.5),
                   cv::Point(m->src.cols / 3 + square_len, m->src.rows / 1.5 + square_len),
                   m->src));
    roi.push_back(
            My_ROI(cv::Point(m->src.cols / 2, m->src.rows / 2),
                   cv::Point(m->src.cols / 2 + square_len, m->src.rows / 2 + square_len),
                   m->src));
    roi.push_back(
            My_ROI(cv::Point(m->src.cols / 2.5, m->src.rows / 2.5),
                   cv::Point(m->src.cols / 2.5 + square_len, m->src.rows / 2.5 + square_len),
                   m->src));
    roi.push_back(
            My_ROI(cv::Point(m->src.cols / 2, m->src.rows / 1.5),
                   cv::Point(m->src.cols / 2 + square_len, m->src.rows / 1.5 + square_len),
                   m->src));
    roi.push_back(
            My_ROI(cv::Point(m->src.cols / 2.5, m->src.rows / 1.8),
                   cv::Point(m->src.cols / 2.5 + square_len, m->src.rows / 1.8 + square_len),
                   m->src));

    for(int i = 0; i < 50; i++) {
        m->cap >> m->src;
        flip(m->src, m->src, 1);

        for(int j = 0; j < NSAMPLES; j++) {
            roi[j].draw_rectangle(m->src);
        }

        std::string imgText = std::string("Cover rectangles with palm");
        printText(m->src, imgText);

        if(i == 30) {
            //	imwrite("./images/waitforpalm1.jpg",m->src);
        }

        imshow("img1", m->src);
        out << m->src;

        if(cv::waitKey(30) >= 0) break;
    }
}

int getMedian(std::vector<int> val) {
    int median;
    size_t size = val.size();
    sort(val.begin(), val.end());

    if(size  % 2 == 0)  {
        median = val[size / 2 - 1] ;
    } else {
        median = val[size / 2];
    }

    return median;
}


void getAvgColor(MyImage *m, My_ROI roi, int avg[3]) {
    cv::Mat r;
    roi.roi_ptr.copyTo(r);
    std::vector<int> hm;
    std::vector<int> sm;
    std::vector<int> lm;

    // generate vectors
    for(int i = 2; i < r.rows - 2; i++) {
        for(int j = 2; j < r.cols - 2; j++) {
            hm.push_back(r.data[r.channels() * (r.cols * i + j) + 0]) ;
            sm.push_back(r.data[r.channels() * (r.cols * i + j) + 1]) ;
            lm.push_back(r.data[r.channels() * (r.cols * i + j) + 2]) ;
        }
    }

    avg[0] = getMedian(hm);
    avg[1] = getMedian(sm);
    avg[2] = getMedian(lm);
}

void average(MyImage *m) {
    m->cap >> m->src;
    flip(m->src, m->src, 1);

    for(int i = 0; i < 30; i++) {
        m->cap >> m->src;
        flip(m->src, m->src, 1);
        cvtColor(m->src, m->src, ORIGCOL2COL);

        for(int j = 0; j < NSAMPLES; j++) {
            getAvgColor(m, roi[j], avgColor[j]);
            roi[j].draw_rectangle(m->src);
        }

        cvtColor(m->src, m->src, COL2ORIGCOL);
        std::string imgText = "Finding average color of hand";
        printText(m->src, imgText);
        imshow("img1", m->src);

        if(cv::waitKey(30) >= 0) break;
    }
}

void initTrackbars() {
    for(int i = 0; i < NSAMPLES; i++) {
        c_lower[i][0] = 12;
        c_upper[i][0] = 7;
        c_lower[i][1] = 30;
        c_upper[i][1] = 40;
        c_lower[i][2] = 80;
        c_upper[i][2] = 80;
    }

    cv::createTrackbar("lower1", "trackbars", &c_lower[0][0], 255);
    cv::createTrackbar("lower2", "trackbars", &c_lower[0][1], 255);
    cv::createTrackbar("lower3", "trackbars", &c_lower[0][2], 255);
    cv::createTrackbar("upper1", "trackbars", &c_upper[0][0], 255);
    cv::createTrackbar("upper2", "trackbars", &c_upper[0][1], 255);
    cv::createTrackbar("upper3", "trackbars", &c_upper[0][2], 255);
}


void normalizeColors(MyImage *myImage) {
    // copy all boundries read from trackbar
    // to all of the different boundries
    for(int i = 1; i < NSAMPLES; i++) {
        for(int j = 0; j < 3; j++) {
            c_lower[i][j] = c_lower[0][j];
            c_upper[i][j] = c_upper[0][j];
        }
    }

    // normalize all boundries so that
    // threshold is whithin 0-255
    for(int i = 0; i < NSAMPLES; i++) {
        if((avgColor[i][0] - c_lower[i][0]) < 0) {
            c_lower[i][0] = avgColor[i][0] ;
        } if((avgColor[i][1] - c_lower[i][1]) < 0) {

            c_lower[i][1] = avgColor[i][1] ;
        } if((avgColor[i][2] - c_lower[i][2]) < 0) {

            c_lower[i][2] = avgColor[i][2] ;
        } if((avgColor[i][0] + c_upper[i][0]) > 255) {

            c_upper[i][0] = 255 - avgColor[i][0] ;
        } if((avgColor[i][1] + c_upper[i][1]) > 255) {

            c_upper[i][1] = 255 - avgColor[i][1] ;
        } if((avgColor[i][2] + c_upper[i][2]) > 255) {

            c_upper[i][2] = 255 - avgColor[i][2] ;
        }
    }
}

void produceBinaries(MyImage *m) {
    cv::Scalar lowerBound;
    cv::Scalar upperBound;
    cv::Mat foo;

    for(int i = 0; i < NSAMPLES; i++) {
        normalizeColors(m);
        lowerBound = cv::Scalar(avgColor[i][0] - c_lower[i][0] , avgColor[i][1] - c_lower[i][1], avgColor[i][2] - c_lower[i][2]);
        upperBound = cv::Scalar(avgColor[i][0] + c_upper[i][0] , avgColor[i][1] + c_upper[i][1], avgColor[i][2] + c_upper[i][2]);
        m->bwList.push_back(cv::Mat(m->srcLR.rows, m->srcLR.cols, CV_8U));
        inRange(m->srcLR, lowerBound, upperBound, m->bwList[i]);
    }

    m->bwList[0].copyTo(m->bw);

    for(int i = 1; i < NSAMPLES; i++) {
        m->bw += m->bwList[i];
    }

    gpuMedianBlur(m->bw, m->bw);
}

void initWindows(MyImage m) {
    cv::namedWindow("trackbars", CV_WINDOW_KEEPRATIO);
    cv::namedWindow("img1", CV_WINDOW_FULLSCREEN);
}

void showWindows(MyImage m) {
    pyrDown(m.bw, m.bw);
    pyrDown(m.bw, m.bw);
    cv::Rect roi(cv::Point(3 * m.src.cols / 4, 0), m.bw.size());
    std::vector<cv::Mat> channels;
    cv::Mat result;

    for(int i = 0; i < 3; i++)
        channels.push_back(m.bw);

    merge(channels, result);
    result.copyTo(m.src(roi));
    imshow("img1", m.src);
}

int findBiggestContour(std::vector<std::vector<cv::Point>> contours) {
    int indexOfBiggestContour = -1;
    int sizeOfBiggestContour = 0;

    for(int i = 0; i < contours.size(); i++) {
        if(contours[i].size() > sizeOfBiggestContour) {
            sizeOfBiggestContour = contours[i].size();
            indexOfBiggestContour = i;
        }
    }

    return indexOfBiggestContour;
}

void myDrawContours(MyImage *m, HandGesture *hg) {
    drawContours(m->src, hg->hullP, hg->cIdx, cv::Scalar(200, 0, 0), 2, 8,
            std::vector<cv::Vec4i>(), 0, cv::Point());

    rectangle(m->src, hg->bRect.tl(), hg->bRect.br(), cv::Scalar(0, 0, 200));

    std::vector<cv::Mat> channels;
    cv::Mat result;

    for(int i = 0; i < 3; i++)
        channels.push_back(m->bw);

    merge(channels, result);
    //	drawContours(result,hg->contours,hg->cIdx,cv::Scalar(0,200,0),6, 8, vector<Vec4i>(), 0, Point());
    drawContours(result, hg->hullP, hg->cIdx, cv::Scalar(0, 0, 250), 10, 8, std::vector<cv::Vec4i>(), 0, cv::Point());


    for(auto& d : hg->defects[hg->cIdx]) {
        cv::Vec4i& v = d;
        int startidx = v[0];
        cv::Point ptStart(hg->contours[hg->cIdx][startidx]);
        int endidx = v[1];
        cv::Point ptEnd(hg->contours[hg->cIdx][endidx]);
        int faridx = v[2];
        cv::Point ptFar(hg->contours[hg->cIdx][faridx]);
        float depth = v[3] / 256;

        /*
         * line(m->src, ptStart, ptFar, Scalar(0, 255, 0), 1);
         * line(m->src, ptEnd, ptFar, Scalar(0, 255, 0), 1);
         * circle(m->src, ptFar,   4, Scalar(0, 255, 0), 2);
         * circle(m->src, ptEnd,   4, Scalar(0, 0, 255), 2);
         * circle(m->src, ptStart,   4, Scalar(255, 0, 0), 2);
         */

        circle(result, ptFar, 9, cv::Scalar(0, 205, 0), 5);
    }

    // imwrite("./images/contour_defects_before_eliminate.jpg",result);

}

void makeContours(MyImage *m, HandGesture *hg) {
    cv::Mat aBw;
    pyrUp(m->bw, m->bw);
    m->bw.copyTo(aBw);
    findContours(aBw, hg->contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    hg->initVectors();
    hg->cIdx = findBiggestContour(hg->contours);

    if(hg->cIdx != -1) {
        // approxPolyDP( Mat(hg->contours[hg->cIdx]), hg->contours[hg->cIdx], 11, true );
        hg->bRect = boundingRect(cv::Mat(hg->contours[hg->cIdx]));
        convexHull(cv::Mat(hg->contours[hg->cIdx]), hg->hullP[hg->cIdx], false, true);
        convexHull(cv::Mat(hg->contours[hg->cIdx]), hg->hullI[hg->cIdx], false, false);
        approxPolyDP(cv::Mat(hg->hullP[hg->cIdx]), hg->hullP[hg->cIdx], 18, true);

        if(hg->contours[hg->cIdx].size() > 3) {
            convexityDefects(hg->contours[hg->cIdx], hg->hullI[hg->cIdx], hg->defects[hg->cIdx]);
            hg->eleminateDefects(m);
        }

        bool isHand = hg->detectIfHand();
        hg->printGestureInfo(m->src);

        if(isHand) {
            hg->getFingerTips(m);
            hg->drawFingerTips(m);
            myDrawContours(m, hg);
        }
    }
}


int main() {
    MyImage m(0); HandGesture hg;
    init(&m);
    m.cap >> m.src;
    cv::namedWindow("img1", CV_WINDOW_KEEPRATIO);
    out.open("out.avi", CV_FOURCC('M', 'J', 'P', 'G'), 15, m.src.size(), true);
    waitForPalmCover(&m);
    average(&m);
    cv::destroyWindow("img1");
    initWindows(m);
    initTrackbars();

    for(;;) {
        hg.frameNumber++;
        m.cap >> m.src;
        flip(m.src, m.src, 1);
        pyrDown(m.src, m.srcLR);
        blur(m.srcLR, m.srcLR, cv::Size(3, 3));
        cvtColor(m.srcLR, m.srcLR, ORIGCOL2COL);
        produceBinaries(&m);
        cvtColor(m.srcLR, m.srcLR, COL2ORIGCOL);
        makeContours(&m, &hg);
        hg.getFingerNumber(&m);
        showWindows(m);
        out << m.src;

        //imwrite("./images/final_result.jpg",m.src);
        if(cv::waitKey(30) == char('q')) break;
    }

    cv::destroyAllWindows();
    out.release();
    m.cap.release();
    return 0;
}
