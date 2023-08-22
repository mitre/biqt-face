// #######################################################################
// NOTICE
//
// This software (or technical data) was produced for the U.S. Government
// under contract, and is subject to the Rights in Data-General Clause
// 52.227-14, Alt. IV (DEC 2007).
//
// Copyright 2019 The MITRE Corporation. All Rights Reserved.
// #######################################################################

#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"

#include "cvskincolorcbcr.h"

void cvSkinColorCrCb(const cv::Mat &_img, cv::Mat &mask)
{
    double Cx = 109.38;
    double Cy = 152.02;
    double theta = 2.53;
    double ecx = 1.6;
    double ecy = 2.41;
    double a = 25.39;
    double b = 14.03;

    int cols = _img.cols;
    int rows = _img.rows;

    cv::Mat img;
    cvtColor(_img, img, cv::COLOR_BGR2YCrCb );
    mask = cv::Mat::zeros(rows, cols, CV_8U);
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            uchar Cr = img.data[img.step * row + col * 3 + 1];
            uchar Cb = img.data[img.step * row + col * 3 + 2];

            double x = cos(theta) * (Cb - Cx) + sin(theta) * (Cr - Cy);
            double y = -1 * sin(theta) * (Cb - Cx) + cos(theta) * (Cr - Cy);

            double distort =
                pow(x - ecx, 2) / pow(a, 2) + pow(y - ecy, 2) / pow(b, 2);

            if (distort <= 1) {
                mask.data[mask.step * row + col] = (uchar)1;
            }
        }
    }
}
