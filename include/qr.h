// ------------------------
// zrd: zhangr.d.1996@gmail.com

#ifndef _QR_H
#define _QR_H

#include <iostream>
#include <string>
#include <vector>
#include <zbar.h>
#include <opencv2/objdetect.hpp>
#include <opencv2/opencv.hpp>
// #include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ximgproc.hpp>

class QR_Rec {
public:
    QR_Rec(const std::vector<std::string> &nameList);
    std::vector<cv::Mat>& getImgList() {return imgList;}
    std::string doRec(int nbThreshold, const std::vector<double> &cropFactor);
private:
    void preprocess();
    std::vector<cv::Mat> extractAll(int nbThreshold);
    std::string extractCode(const cv::Mat &img);
    std::vector<cv::Mat> imgList;
    std::vector<cv::Mat> edgesImg;
    std::vector<cv::Point2f> extractImg(const cv::Mat &img, int nbThreshold, int imgSize);
    std::vector<cv::Vec4f> extractLines(const cv::Mat &img, int nbThreshold);
    cv::Mat assembleImg(const std::vector<cv::Mat> &rectifiedImgs, double cropFactor, int rotateFactor);
};

#endif  //_QR_H