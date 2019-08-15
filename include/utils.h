// ------------------------
// zrd: zhangr.d.1996@gmail.com

#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

void showHist(const cv::Mat &hist, int maxHeight);
cv::Point2f intersectPoint(const cv::Vec4f &l1, const cv::Vec4f &l2);
void RotateImage(cv::Mat &src, cv::Mat &dst, double angle);

#endif  //_UTILS_H