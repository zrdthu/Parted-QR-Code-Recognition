// ------------------------
// zrd: zhangr.d.1996@gmail.com

#include "utils.h"

void showHist(const cv::Mat &hist, int maxHeight) {
    double max_val;
    cv::minMaxLoc(hist, 0, &max_val, 0, 0);
    int hist_height = 256;
    int binNum = hist.size().height;
    cv::Mat hist_img = cv::Mat::zeros(hist_height, binNum, CV_8UC3);
    for(int i = 0; i < binNum; i++) {
        float bin_val = hist.at<float>(i);
        int intensity = cvRound(bin_val * hist_height / max_val);  //要绘制的高度
        rectangle(hist_img, cv::Point(i, hist_height - 1),
            cv::Point(i, hist_height - intensity),
            CV_RGB(255,255,255));
    }
    imshow("Hist", hist_img);
    cv::waitKey(0);
}

cv::Point2f intersectPoint(const cv::Vec4f &l1, const cv::Vec4f &l2) {
    float vx1 = l1[0], vy1 = l1[1], x1 = l1[2], y1 = l1[3];
    float vx2 = l2[0], vy2 = l2[1], x2 = l2[2], y2 = l2[3];
    float y = (vx1*vy2*y1 - vx2*vy1*y2 - vy1*vy2*(x1-x2)) / (vx1*vy2 - vx2*vy1);
    float x = (vx2*vy1*x1 - vx1*vy2*x2 - vx1*vx2*(y1-y2)) / (vx2*vy1 - vx1*vy2);
    return cv::Point2f(x, y);
}

void RotateImage(cv::Mat &src, cv::Mat &dst, double angle) {
	try {
		cv::Size dst_sz(src.cols, src.rows);  
		cv::Point2f center(static_cast<float>(src.cols / 2.), static_cast<float>(src.rows / 2.));
		cv::Mat rot_mat = cv::getRotationMatrix2D(center, angle, 1.0);
		cv::warpAffine(src, dst, rot_mat, dst_sz, cv::INTER_LINEAR, cv::BORDER_REPLICATE);
	}
	catch (cv::Exception e) {
	}
}

/*
y - y1 = vy/vx (x - x1)
vx1(y - y1) = vy1(x - x1)
vx2(y - y2) = vy2(x - x2)

vy2 vx1(y - y1) = vy1 vy2(x - x1)
vy1 vx2(y - y2) = vy1 vy2(x - x2)

(vx1 vy2 - vx2 vy1)y -vx1vy2y1 +vx2vy1y2 = -vy1vy2x1 +vy1vy2x2

y = (vx1vy2y1-vx2vy1y2 -vy1vy2(x1-x2)) / (vx1 vy2 - vx2 vy1)


vx1 vx2(y - y1) = vx2 vy1(x - x1)
vx1 vx2(y - y2) = vx1 vy2(x - x2)

vx1vx2(-y1+y2) = (vx2vy1 - vx1vy2)x -vx2vy1x1+vx1vy2x2
x = (vx2vy1x1 - vx1vy2x2 - vx1vx2(y1-y2)) / (vx2vy1 - vx1vy2)


*/