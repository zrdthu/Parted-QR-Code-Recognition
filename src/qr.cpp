// ------------------------
// zrd: zhangr.d.1996@gmail.com

#include "qr.h"
#include "utils.h"

using namespace std;
// using namespace cv;

QR_Rec::QR_Rec(const vector<string> &nameList) {
    for (auto &filename : nameList) {
        imgList.push_back(cv::imread(filename));
    }
}

void QR_Rec::preprocess() {
    for (auto &img : imgList) {
        cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
        double scale = 400. / img.rows;
        cv::resize(img, img, cv::Size(img.cols * scale, img.rows * scale));
        cv::blur(img, img, cv::Size(5, 5));

        // cv::imwrite("blur.png", img);

        // cv::Mat grayImg;
        img.convertTo(img, CV_32FC1);
        img /= 255.F;
        // imshow("Gray", grayImg);
        // cv::Canny(img, img, 40, 100, 3);
        

        cv::Mat gH, gV;
        float k1[3][1] = {-1, 0, 1};  //水平方向的核
        float k2[] = {-1, 0, 1};  //垂直的核
        cv::Mat kH(1, 3, CV_32FC1, k1);
        cv::Mat kV(3, 1, CV_32FC1, k2);

        // cout << kH << endl << kV << endl;
        cv::filter2D(img, gH, -1, kH);    //水平卷积运算
        cv::filter2D(img, gV, -1, kV);

        cv::Mat mag, ang;
        cv::cartToPolar(gH, gV, mag, ang);

        ang = ang * 180 / CV_PI;

        // imshow("Mag", mag);
        mag *= 255;
        cv::Mat ucharGray, bin;
        mag.convertTo(ucharGray, CV_8UC1);
        // cv::imwrite("h.png", ucharGray);
        cv::threshold(ucharGray, bin, 0, 255, cv::THRESH_OTSU);
        // cv::imwrite("bw.png", bin);

        // ---------------- calc hist ----------------
        int binNum = 360;
        float range[] = {0, 360};
        const float *histRange = {range};
        cv::Mat hist;
        // use bin as mask
        cv::calcHist(&ang, 1, nullptr, bin, hist, 1, &binNum, &histRange);
        // showHist(hist, 256);
        cv::Point maxLoc;
        cv::Mat histMask(binNum, 1, CV_8UC1, (unsigned char)0);
        histMask(cv::Rect(0, 1, 1, 45)) = 1;
        histMask(cv::Rect(0, 313, 1, 45)) = 1;
        cv::minMaxLoc(hist, nullptr, nullptr, nullptr, &maxLoc, histMask);
        int maxIdx = maxLoc.y;
        maxIdx -= (maxIdx >= 45) * 360;
        RotateImage(img, img, maxIdx);
        RotateImage(bin, bin, maxIdx);
        // cout << maxIdx << endl;
        // -------------------------------------------

        cv::Mat openKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::morphologyEx(bin, bin, cv::MORPH_OPEN, openKernel);
        cv::morphologyEx(bin, bin, cv::MORPH_CLOSE, openKernel);
        cv::ximgproc::thinning(bin, bin);
        // cv::imwrite("thin.png", bin);
        edgesImg.push_back(bin);
        // cin.get();
        // system("pause");
        // cv::imwrite("edges.png", bin);

        // imshow("Bin", bin);

        // cv::Mat thin;
        // imshow("Thin", bin);


        // vector<cv::Vec2f> lines;
        // cv::HoughLines(bin, lines, 1, CV_PI / 180, 50, 0, 0, -CV_PI / 4, CV_PI / 4);

        // for (auto &line : lines) {
        //     float rho = line[0], theta = line[1];
        //     cv::Point pt1, pt2;
        //     double a = cos(theta), b = sin(theta);
        //     double x0 = a*rho, y0 = b*rho;
        //     pt1.x = cvRound(x0 + 1000*(-b));
        //     pt1.y = cvRound(y0 + 1000*(a));
        //     pt2.x = cvRound(x0 - 1000*(-b));
        //     pt2.y = cvRound(y0 - 1000*(a));
        //     cv::line(grayImg, pt1, pt2, cv::Scalar(55,100,195));
        // }
        // cv::imshow("Detected", grayImg);
        // cv::waitKey(0);
    }
}

vector<cv::Vec4f> QR_Rec::extractLines(const cv::Mat &img, int nbThreshold) {

    int h = img.size().height;
    int w = img.size().width;

    vector<cv::Point2i> edgeListLeft, edgeListRight;
    int leftMost = w, rightMost = 0;
    for (int i = 0; i < h; i++) {
        bool leftFound = false, rightFound = false;
        for (int j = 0; j < w; j++) {
            if (!leftFound && img.at<unsigned char>(i, j)) {
                if (j <= leftMost + nbThreshold) {
                    if (j < leftMost - nbThreshold)
                        edgeListLeft.clear();
                    edgeListLeft.push_back(cv::Point2i(i, j));
                    leftMost = j;
                    leftFound = true;
                }
            }
            if (!rightFound && img.at<unsigned char>(i, w - 1 - j)) {
                if (w - 1 - j >= rightMost - nbThreshold) {
                    // cout << 
                    if (w - 1 - j > rightMost + nbThreshold)
                        edgeListRight.clear();
                    edgeListRight.push_back(cv::Point2i(i, w - 1 - j));
                    rightMost = w - 1 - j;
                    rightFound = true;
                }
            }
        }
    }

    vector<cv::Point2i> edgeListTop, edgeListBottom;
    int topMost = w, bottomMost = 0;
    for (int i = 0; i < w; i++) {
        bool topFound = false, bottomFound = false;
        for (int j = 0; j < h; j++) {
            if (!topFound && img.at<unsigned char>(j, i)) {
                if (j <= topMost + nbThreshold) {
                    if (j < topMost - nbThreshold)
                        edgeListTop.clear();
                    edgeListTop.push_back(cv::Point2i(j, i));
                    topMost = j;
                    topFound = true;
                }
            }
            if (!bottomFound && img.at<unsigned char>(h - 1 - j, i)) {
                if (h - 1 - j >= bottomMost - nbThreshold) {
                    if (h - 1 - j > bottomMost + nbThreshold)
                        edgeListBottom.clear();
                    edgeListBottom.push_back(cv::Point2i(h - 1 - j, i));
                    bottomMost = h - 1 - j;
                    bottomFound = true;
                }
            }
        }
    }
    vector<cv::Vec4f> lines(4, cv::Vec4f(0, 0, 0, 0));
    cv::fitLine(edgeListLeft, lines[0], cv::DIST_L2, 0, 0.01, 0.01);
    cv::fitLine(edgeListTop, lines[1], cv::DIST_L2, 0, 0.01, 0.01);
    cv::fitLine(edgeListRight, lines[2], cv::DIST_L2, 0, 0.01, 0.01);
    cv::fitLine(edgeListBottom, lines[3], cv::DIST_L2, 0, 0.01, 0.01);
    return lines;    
}

vector<cv::Point2f> QR_Rec::extractImg(const cv::Mat &img, int nbThreshold, int imgSize) {
    vector<cv::Vec4f> lines = extractLines(img, nbThreshold);
    vector<cv::Point2f> points;
    for (int i = 0; i < 4; i++)
        points.push_back(intersectPoint(lines[i], lines[(i+1)%4]));
    
    // cv::Mat dottedImg(img);
    // dottedImg /= 4;
    // for (auto &p : points) {
    //     dottedImg.at<unsigned char>(int(p.x), int(p.y)) = 255;
    // }
    // // for (auto &p : edgeListLeft)
    // //     dottedImg.at<unsigned char>(p.x, p.y) = 128;
    // // for (auto &p : edgeListRight)
    // //     dottedImg.at<unsigned char>(p.x, p.y) = 128;
    // // for (auto &p : edgeListTop)
    // //     dottedImg.at<unsigned char>(p.x, p.y) = 128;
    // // for (auto &p : edgeListBottom)
    // //     dottedImg.at<unsigned char>(p.x, p.y) = 128;
    // cv::imshow("intsect point", dottedImg);
    // cv::waitKey();
    // cout << lines[0] << lines[1] << lines[2] << lines[3] << endl;
    return points;
}

vector<cv::Mat> QR_Rec::extractAll(int nbThreshold) {
    vector<cv::Mat> rectifiedImgs(4, cv::Mat());
    #pragma omp parallel for
    for (int i = 0; i < 4; i++) {
        cv::Mat &img = edgesImg[i];
        int w = img.size().width;
        int h = img.size().height;
        vector<cv::Point2f> points = extractImg(img, nbThreshold, 200);
        cv::Point2f srcPoints[] = {points[0], points[1], points[2], points[3]};
        cv::Point2f dstPoints[] = {cv::Point2f(0, 0), cv::Point2f(0, w), cv::Point2f(h, w), cv::Point2f(h, 0)};
        cv::Mat psptvMtx = cv::getPerspectiveTransform(srcPoints, dstPoints);
        // cv::Mat rImg;
        cv::warpPerspective(imgList[i].t(), rectifiedImgs[i], psptvMtx, cv::Size(h, w));
        cv::resize(rectifiedImgs[i].t(), rectifiedImgs[i], cv::Size(200, 200));
        rectifiedImgs[i] *= 255;
        rectifiedImgs[i].convertTo(rectifiedImgs[i], CV_8UC1);
        cv::threshold(rectifiedImgs[i], rectifiedImgs[i], 0, 255, cv::THRESH_OTSU);
        // cv::imshow("Rectified", rectifiedImgs[i]);
        // cv::waitKey(0);
        // rImg.copyTo(rectifiedImg(cv::Range(xl[i], xl[i] + 200), cv::Range(yl[i], yl[i] + 200)));
    }
    return rectifiedImgs;
}

cv::Mat QR_Rec::assembleImg(const vector<cv::Mat> &rectifiedImgs, double cropFactor, int rotateFactor) {
    int cropSize = (200. / cropFactor);
    cv::Mat ans(4 * cropSize, 4 * cropSize, CV_8UC1, (unsigned char)255);
    // cv::imshow("r", rectifiedImgs[0]);
    // cv::waitKey(0);
    cv::Mat newPart4(rectifiedImgs[3]);
    if (rotateFactor < 3)
        cv::rotate(rectifiedImgs[3], newPart4, rotateFactor);
    rectifiedImgs[0](cv::Range(0 , cropSize), cv::Range(0, cropSize)).copyTo(ans(cv::Range(cropSize, 2 * cropSize), cv::Range(cropSize, 2 * cropSize)));
    rectifiedImgs[1](cv::Range(0, cropSize), cv::Range(200 - cropSize, 200)).copyTo(ans(cv::Range(cropSize, 2 * cropSize), cv::Range(2 * cropSize, 3 * cropSize)));
    newPart4(cv::Range(200 - cropSize, 200), cv::Range(200 - cropSize, 200)).copyTo(ans(cv::Range(2 * cropSize, 3 * cropSize), cv::Range(2 * cropSize, 3 * cropSize)));
    rectifiedImgs[2](cv::Range(200 - cropSize, 200), cv::Range(0, cropSize)).copyTo(ans(cv::Range(2 * cropSize, 3 * cropSize), cv::Range(cropSize, 2 * cropSize)));
    return ans;
}

string QR_Rec::extractCode(const cv::Mat &img) {
    cv::QRCodeDetector qrDecoder;
    // cv::Mat bbox, rectified;
    return qrDecoder.detectAndDecode(img);
    // zbar::ImageScanner scanner;
    // // configure the reader
    // scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
    // int w = img.size().width;
    // int h = img.size().height;
    // // cv::Mat ucharImg(img.size(), CV_8UC1), img255(255 * img);
    // // // ucharImg *= 255;
    // // img255.convertTo(ucharImg, CV_8UC1);
    // // cout << ucharImg << endl;
    // unsigned char *pdata = (unsigned char *)img.data;
    // zbar::Image imageZbar(w, h, "Y800", pdata, w * h);
    // int n = scanner.scan(imageZbar);

    // std::string ans = "";

    // if (n > 0) {
    //     zbar::Image::SymbolIterator symbol = imageZbar.symbol_begin();
    //     ans = symbol->get_data();
    // }
    // imageZbar.set_data(nullptr, 0);
    // return ans;
}

string QR_Rec::doRec(int nbThreshold, const vector<double> &cropFactors) {
    preprocess();
    vector<cv::Mat> rectifiedImgs = extractAll(nbThreshold);
    cv::Mat assembledImg;
    string ans = "";
    // vector<vector<int> > rotates({
    //     {-1, -1, -1, -1},                       // 0 0 0 0
    //     {-1, -1, -1, cv::ROTATE_90_CLOCKWISE},  // 0 0 0 1
    //     {-1, -1, -1, cv::ROTATE_90_CLOCKWISE},  // 0 0 0 2
    //     {-1, -1, -1, cv::ROTATE_90_CLOCKWISE},  // 0 0 0 3
    //     {-1, -1, cv::ROTATE_90_CLOCKWISE, -1},  // 0 0 1 3
    //     {-1, -1, cv::ROTATE_90_CLOCKWISE, -1},  // 0 0 2 3
    //     {-1, -1, cv::ROTATE_90_CLOCKWISE, -1},  // 0 0 3 3
    //     {-1, -1, -1, },  // 0 0 3 3
    // });
    for (int n = 0; n < 3; n++) {
        const cv::Mat &img = rectifiedImgs[n];
        double x = 0, y = 0, w = 0;
        for (int i = 0; i < img.size().height; i++)
            for (int j = 0; j < img.size().width; j++)
                if (img.at<uchar>(i, j)) {
                    x += double(i);
                    y += double(j);
                    w += 1.;
                }
        x /= w;
        y /= w;
        if (n == 0) {
            if (x < 100 && y > 100)
                cv::rotate(rectifiedImgs[0], rectifiedImgs[0], cv::ROTATE_90_CLOCKWISE);
            else if (x < 100 && y < 100)
                cv::rotate(rectifiedImgs[0], rectifiedImgs[0], cv::ROTATE_180);
            else if (x > 100 && y < 100)
                cv::rotate(rectifiedImgs[0], rectifiedImgs[0], cv::ROTATE_90_COUNTERCLOCKWISE);
        }
        else if (n == 1) {
            if (x > 100 && y > 100)
                cv::rotate(rectifiedImgs[1], rectifiedImgs[1], cv::ROTATE_90_CLOCKWISE);
            else if (x < 100 && y > 100)
                cv::rotate(rectifiedImgs[1], rectifiedImgs[1], cv::ROTATE_180);
            else if (x < 100 && y < 100)
                cv::rotate(rectifiedImgs[1], rectifiedImgs[1], cv::ROTATE_90_COUNTERCLOCKWISE);
        }
        else if (n == 2) {
            if (x < 100 && y < 100)
                cv::rotate(rectifiedImgs[2], rectifiedImgs[2], cv::ROTATE_90_CLOCKWISE);
            else if (x > 100 && y < 100)
                cv::rotate(rectifiedImgs[2], rectifiedImgs[2], cv::ROTATE_180);
            else if (x > 100 && y > 100)
                cv::rotate(rectifiedImgs[2], rectifiedImgs[2], cv::ROTATE_90_COUNTERCLOCKWISE);
        }
        // cout << img.size().height << ' ' << x << ' ' << img.size().width << ' ' << y << endl;
    }
// #pragma omp parallel for
    for (auto factor : cropFactors)
        for (int r = 0; r < 4; r++) {
            assembledImg = assembleImg(rectifiedImgs, factor, r);
            // cv::imwrite("assembled.png", assembledImg);
            // cv::imshow("Assembled", assembledImg);
            // cv::waitKey();
            ans = extractCode(assembledImg);
            if (ans.length())
                return ans;
    }
    return "Failed";
}
