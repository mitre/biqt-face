// #######################################################################
// NOTICE
//
// This software (or technical data) was produced for the U.S. Government
// under contract, and is subject to the Rights in Data-General Clause
// 52.227-14, Alt. IV (DEC 2007).
//
// Copyright 2019 The MITRE Corporation. All Rights Reserved.
// #######################################################################

#include "Face.h"
#include "cvskincolorcbcr.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <fstream>
#include <iostream>
#include <string>

Face::Face() {}
Face::~Face() {}

void Face::setMetricsWriteMap(std::string name, int index)
{
    metricsWriteMap[index] = name;
}

void Face::initializeMetricsWriteMap()
{
    setMetricsWriteMap("ImageWidth", ImageWidth);
    setMetricsWriteMap("ImageHeight", ImageHeight);
    setMetricsWriteMap("ImageChannels", ImageChannels);
    setMetricsWriteMap("ImageArea", ImageArea);
    setMetricsWriteMap("ImageRatio", ImageRatio);

    setMetricsWriteMap("CvFrontalFaceFound", CvFrontalFaceFound);
    setMetricsWriteMap("CvProfileFaceFound", CvProfileFaceFound);
    setMetricsWriteMap("CvFaceX", CvFaceX);
    setMetricsWriteMap("CvFaceY", CvFaceY);
    setMetricsWriteMap("CvFaceWidth", CvFaceWidth);
    setMetricsWriteMap("CvFaceHeight", CvFaceHeight);

    setMetricsWriteMap("SAPLevel", SAPLevel);
    setMetricsWriteMap("SAPFailureCode", SAPFailureCode);

    setMetricsWriteMap("CvNumLandmarks", CvNumLandmarks);
    setMetricsWriteMap("CvEyeCount", CvEyeCount);
    setMetricsWriteMap("CvNoseCount", CvNoseCount);
    setMetricsWriteMap("CvMouthCount", CvMouthCount);

    setMetricsWriteMap("FaceCenterOfMassX", FaceCenterOfMassX);
    setMetricsWriteMap("FaceCenterOfMassY", FaceCenterOfMassY);
    setMetricsWriteMap("FaceOffsetX", FaceOffsetX);
    setMetricsWriteMap("FaceOffsetY", FaceOffsetY);

    setMetricsWriteMap("SkinFull", SkinFull);
    setMetricsWriteMap("SkinFace", SkinFace);

    setMetricsWriteMap("Focus", Focus);
    setMetricsWriteMap("FocusFace", FocusFace);
    setMetricsWriteMap("Blur", Blur);
    setMetricsWriteMap("BlurFace", BlurFace);
    setMetricsWriteMap("OverExposure", OverExposure);
    setMetricsWriteMap("OverExposureFace", OverExposureFace);

    // OpenCV Landmarks
    setMetricsWriteMap("CvRightEyePosition_X", CvRightEyePosition_X);
    setMetricsWriteMap("CvRightEyePosition_Y", CvRightEyePosition_Y);
    setMetricsWriteMap("CvLeftEyePosition_X", CvLeftEyePosition_X);
    setMetricsWriteMap("CvLeftEyePosition_Y", CvLeftEyePosition_Y);
    setMetricsWriteMap("CvNosePosition_X", CvNosePosition_X);
    setMetricsWriteMap("CvNosePosition_Y", CvNosePosition_Y);
    setMetricsWriteMap("CvMouthPosition_X", CvMouthPosition_X);
    setMetricsWriteMap("CvMouthPosition_Y", CvMouthPosition_Y);
    setMetricsWriteMap("CvIPD", CvIPD);

    // OpenBR
    setMetricsWriteMap("BrRightEyePosition_X", BrRightEyePosition_X);
    setMetricsWriteMap("BrRightEyePosition_Y", BrRightEyePosition_Y);
    setMetricsWriteMap("BrLeftEyePosition_X", BrLeftEyePosition_X);
    setMetricsWriteMap("BrLeftEyePosition_Y", BrLeftEyePosition_Y);
    setMetricsWriteMap("BrIPD", BrIPD);

    // background
    setMetricsWriteMap("BGDeviation", BGDeviation);
    setMetricsWriteMap("BGGrayness", BGGrayness);
}

bool Face::initialize(const std::string biqtPath)
{
    // creates the std::map containing std::string values for each metric
    initializeMetricsWriteMap();
    if (!cvLandmarker.initialize(biqtPath)) {
        return false;
    }

    brLandmarker.initialize(biqtPath);
    // nothing to return from openbr - it will print out reasons for failure
    return true;
}

void Face::finalize()
{
    // finalize happens in the destructors of the landmarkers being referenced
}

double Face::calculateDistance(int x1, int y1, int x2, int y2)
{
    if (x1 == -1 || y1 == -1 || x2 == -1 || y2 == -1) {
        return (double)-1;
    }
    else {
        double squareOneDist = pow(((double)x2 - (double)x1), 2);
        double squareTwoDist = pow(((double)y2 - (double)y1), 2);
        return sqrt((squareOneDist + squareTwoDist));
    }
}

/*
 *Image Metrics
 */
void Face::setWidth(const cv::Mat &img, std::map<std::string, double> &metrics)
{
    metrics["ImageWidth"] = (int)img.cols;
}

void Face::setHeight(const cv::Mat &img, std::map<std::string, double> &metrics)
{
    metrics["ImageHeight"] = (int)img.rows;
}

void Face::setChannels(const cv::Mat &img,
                       std::map<std::string, double> &metrics)
{
    metrics["ImageChannels"] = (double)img.channels();
}

void Face::setArea(const cv::Mat &img, std::map<std::string, double> &metrics)
{
    metrics["ImageArea"] = (double)img.rows * (double)img.cols;
}

void Face::setRatio(const cv::Mat &img, std::map<std::string, double> &metrics)
{
    metrics["ImageRatio"] = (double)img.cols / (double)img.rows;
}

/*
 *Face Metrics
 */
void Face::setFace(const cv::Mat &img, std::map<std::string, double> &metrics,
                   const std::vector<CvLandmarker::LandmarkFace> &landmarkFaces)
{
    // there won't be more than one face found - the cvlandmarker finds the
    // largest face
    if (landmarkFaces.size() > 0) {
        metrics["CvFaceFound"] = true;
        cv::Rect faceRect = landmarkFaces[0].faceRect;

        if (faceRect.area() == 0) {
            return;
        }

        // there will only be one face rect - cvlandmarker is set for largest
        // rect
        for (unsigned int i = 0; i < landmarkFaces.size(); i++) {
            if (landmarkFaces[i].isProfile) {
                metrics["CvProfileFaceFound"] += 1;
            }
            else {
                metrics["CvFrontalFaceFound"] += 1;
                // not setting face rect for
                metrics["CvFaceX"] = faceRect.x;
                metrics["CvFaceY"] = faceRect.y;
                metrics["CvFaceWidth"] = faceRect.width;
                metrics["CvFaceHeight"] = faceRect.height;
            }
        }
    }
    else {
        metrics["CvFaceFound"] = false;
    }
}

void Face::setCvNumLandmarks(std::map<std::string, double> &metrics,
                             const CvLandmarker::LandmarkFace &landmarkFace)
{
    if (biqtMode == FULL) {
        int numLandmarks = landmarkFace.numLandmarks;
        metrics["CvNumLandmarks"] = numLandmarks;
    }
}

void Face::setEyeCount(const cv::Mat &img,
                       std::map<std::string, double> &metrics,
                       const CvLandmarker::LandmarkFace &landmarkFace)
{
    if (metrics["CvFrontalFaceFound"] <= 0) {
        return;
    }

    if (biqtMode != LANDMARK) {
        if (landmarkFace.leftEye.x > -1 && landmarkFace.leftEye.y > -1) {
            metrics["CvEyeCount"]++;
        }

        if (landmarkFace.rightEye.x > -1 && landmarkFace.rightEye.y > -1) {
            metrics["CvEyeCount"]++;
        }
    }

    // set the vertical and horizontal position of the eyes
    // the landmark face points will be initialized to -1,-1
    metrics["CvRightEyePosition_X"] = landmarkFace.rightEye.x;
    metrics["CvRightEyePosition_Y"] = landmarkFace.rightEye.y;
    metrics["CvLeftEyePosition_X"] = landmarkFace.leftEye.x;
    metrics["CvLeftEyePosition_Y"] = landmarkFace.leftEye.y;

    // if we have two eyes we can get the distance between them
    if (biqtMode != LANDMARK && metrics["CvEyeCount"] == 2) {
        // if we have two eyes we can get the distance
        metrics["CvIPD"] = calculateDistance(
            metrics["CvRightEyePosition_X"], metrics["CvRightEyePosition_Y"],
            metrics["CvLeftEyePosition_X"], metrics["CvLeftEyePosition_Y"]);
    }
}

void Face::setNoseCount(const cv::Mat &img,
                        std::map<std::string, double> &metrics,
                        const CvLandmarker::LandmarkFace &landmarkFace)
{
    if (metrics["CvFrontalFaceFound"] <= 0) {
        return;
    }

    // they'll be initialized to -1, this is fine
    metrics["CvNosePosition_X"] = landmarkFace.noseTip.x;
    metrics["CvNosePosition_Y"] = landmarkFace.noseTip.y;

    if (biqtMode != LANDMARK) {
        if (metrics["CvNosePosition_X"] > -1 &&
            metrics["CvNosePosition_Y"] > -1) {
            metrics["CvNoseCount"]++;
        }
    }
}

void Face::setMouthCount(const cv::Mat &img,
                         std::map<std::string, double> &metrics,
                         const CvLandmarker::LandmarkFace &landmarkFace)
{
    if (metrics["CvFrontalFaceFound"] <= 0) {
        return;
    }

    // they'll be initialized to -1, this is fine
    metrics["CvMouthPosition_X"] = landmarkFace.mouth.x;
    metrics["CvMouthPosition_Y"] = landmarkFace.mouth.y;

    if (biqtMode != LANDMARK) {
        if (metrics["CvMouthPosition_X"] > -1 &&
            metrics["CvMouthPosition_Y"] > -1) {
            metrics["CvMouthCount"]++;
        }
    }
}

void Face::getCircularROI(int R, std::vector<int> &RxV)
{
    RxV.resize(R + 1);
    for (int y = 0; y <= R; y++) {
        RxV[y] = cvRound(sqrt((double)R * R - y * y));
    }
}

/*
 * only called in FULL mode
 * useFaceRect lets you choose to use only that region when determing the
 * metric, if no face region exists it will use the whole image
 */
void Face::setOverExposure(const cv::Mat &img,
                           std::map<std::string, double> &metrics,
                           bool useFaceRect)
{
    if (img.channels() != 3) {
        // this would be an error
        metrics["OverExposure"] = -1;
        metrics["OverExposureFace"] = -1;
        return;
    }

    const double sigma = 1.0 / 60.0;
    const int Lthresh = 80, Cthresh = 40;

    cv::Mat img_lab;

    cv::Rect mask = cv::Rect(0, 0, img.cols, img.rows);
    if (useFaceRect) {
        if (metrics["CvFrontalFaceFound"] < 1) {
            metrics["OverExposureFace"] = -1;
            return;
        }
        // we know we want to use the face rect and have a frontal face found
        mask =
            cv::Rect((int)metrics["CvFaceX"], (int)metrics["CvFaceY"],
                     (int)metrics["CvFaceWidth"], (int)metrics["CvFaceHeight"]);
    }

    cv::Mat maskImg = img(mask);
    cvtColor(maskImg, img_lab, CV_BGR2Lab);

    std::vector<cv::Mat> planes_lab;
    split(img_lab, planes_lab);

    int good = 0, bad = 0;
    for (int y = 0; y < maskImg.rows; y++) {
        for (int x = 0; x < maskImg.cols; x++) {
            cv::Vec2i ab(planes_lab[1].at<uchar>(y, x),
                         planes_lab[2].at<uchar>(y, x));
            int L = planes_lab[0].at<uchar>(y, x);
            double P =
                0.5 *
                (tanh(sigma * ((L - Lthresh) + (Cthresh - norm(ab)))) + 1.0);
            if (P > 0.5)
                bad++;
            else
                good++;
        }
    }

    if (useFaceRect) {
        metrics["OverExposureFace"] =
            (good + bad > 0) ? (double)bad / (double)(good + bad) : 0.0;
    }
    else {
        metrics["OverExposure"] =
            (good + bad > 0) ? (double)bad / (double)(good + bad) : 0.0;
    }
}

/*
 * only called in FULL mode
 * useFaceRect lets you choose to use only that region when determing the
 * metric, if no face region exists it will use the whole image
 */
void Face::setFocus(const cv::Mat &img, std::map<std::string, double> &metrics,
                    bool useFaceRect)
{
    const int aperture_size = 7;
    cv::Mat imgNew = img;
    // not convertin the image to gray
    // the blue channel or the first channel (BGR)
    // is a better metric than when using the grayscale image
    if (img.channels() == 1) {
        equalizeHist(img, imgNew);
    }

    cv::Mat src = imgNew;
    if (useFaceRect) {
        if (metrics["CvFrontalFaceFound"] < 1) {
            metrics["FocusFace"] = -1;
            return;
        }
        src = imgNew(cv::Rect((int)metrics["CvFaceX"], (int)metrics["CvFaceY"],
                              (int)metrics["CvFaceWidth"],
                              (int)metrics["CvFaceHeight"]));
    }

    cv::Mat x, y;
    Sobel(src, x, CV_32F, 1, 0, aperture_size);
    Sobel(src, y, CV_32F, 0, 1, aperture_size);

    cv::Mat m;
    magnitude(x, y, m);

    if (useFaceRect && metrics["CvFrontalFaceFound"] > 0) {
        metrics["FocusFace"] = (double)mean(m)[0];
    }
    else {
        metrics["Focus"] = (double)mean(m)[0];
    }
}

void Face::setFaceOffset(const cv::Mat &img,
                         std::map<std::string, double> &metrics)
{
    // calculate skin of the entire image
    if (biqtMode == FULL) {
        cv::Mat maskImg;
        cvSkinColorCrCb(img, maskImg);
        metrics["SkinFull"] =
            (float)sum(maskImg)[0] / (maskImg.rows * maskImg.cols);
    }

    if (metrics["CvFrontalFaceFound"] == 1) {
        cv::Rect roi =
            cv::Rect((int)metrics["CvFaceX"], (int)metrics["CvFaceY"],
                     (int)metrics["CvFaceWidth"], (int)metrics["CvFaceHeight"]);
        cv::Mat face = img(roi);
        cv::Mat mask;
        cvSkinColorCrCb(face, mask);
        metrics["SkinFace"] = (float)sum(mask)[0] / (mask.rows * mask.cols);

        if (biqtMode == FULL) {
            cv::Moments m = moments(mask, true);
            float xCenter = m.m10 / m.m00;
            float yCenter = m.m01 / m.m00;

            if (xCenter > 0 && yCenter > 0) {
                metrics["FaceCenterOfMassX"] = roi.x + xCenter;
                metrics["FaceCenterOfMassY"] = roi.y + yCenter;
            }
            else {
                metrics["FaceCenterOfMassX"] = -1.0;
                metrics["FaceCenterOfMassY"] = -1.0;
            }

            double imgMidX = img.cols * 0.5;
            double imgMidY = img.rows * 0.5;
            metrics["FaceOffsetX"] =
                ((roi.x + roi.width * 0.5) - imgMidX) / imgMidX;
            metrics["FaceOffsetY"] =
                (imgMidY - (roi.y + roi.height * 0.5)) / imgMidY;
        }
    }
}

/*
 * only called in FUll mode
 */
void Face::setBackground(const cv::Mat &img,
                         std::map<std::string, double> &metrics)
{
    int posX = floor(metrics["ImageWidth"] * .95) - 1;
    int posY = floor(metrics["ImageHeight"] * .95) - 1;
    int width = metrics["ImageWidth"] - 1 - posX;
    int height = metrics["ImageHeight"] - 1 - posY;

    // check the ul and ur corners of image
    cv::Rect roi[2];
    roi[0] = cv::Rect(0, 0, ceil(metrics["ImageWidth"] * 0.05),
                      ceil(metrics["ImageHeight"] * 0.05));
    roi[1] = cv::Rect(posX, 0, width, height);

    double worstDev = -1;
    double worstColorDiff = -1;
    for (int i = 0; i < 2; i++) {
        // Create an img at the roi and split the rgb channels
        cv::Mat temp = img(roi[i]);
        std::vector<cv::Mat> splitChannels;
        split(temp, splitChannels);
        cv::Scalar means[3];
        cv::Scalar stdDevs[3];

        for (int j = 0; j < 3; j++) {
            meanStdDev(splitChannels[j], means[j], stdDevs[j]);

            if ((double)stdDevs[j][0] > worstDev) {
                worstDev = (double)stdDevs[j][0];
            }
        }

        if (abs(means[0][0] - means[1][0]) > worstColorDiff) {
            worstColorDiff = abs(means[0][0] - means[1][0]);
        }
        if (abs(means[0][0] - means[2][0]) > worstColorDiff) {
            worstColorDiff = abs(means[0][0] - means[2][0]);
        }
        if (abs(means[1][0] - means[2][0]) > worstColorDiff) {
            worstColorDiff = abs(means[1][0] - means[2][0]);
        }
    }

    metrics["BGDeviation"] = worstDev;
    metrics["BGGrayness"] = worstColorDiff;
}

/*
 * only called in FULL mode
 * useFaceRect lets you choose to use only that region when determing the
 * metric, if no face region exists it will use the whole image
 */
void Face::setBlur(const cv::Mat &img, std::map<std::string, double> &metrics,
                   bool useFaceRect)
{
    const int diffThreshold = 10;

    cv::Mat blur0 = img; // original image
    if (useFaceRect) {
        if (metrics["CvFrontalFaceFound"] < 1) {
            metrics["BlurFace"] = -1;
            return;
        }
        blur0 = img(cv::Rect((int)metrics["CvFaceX"], (int)metrics["CvFaceY"],
                             (int)metrics["CvFaceWidth"],
                             (int)metrics["CvFaceHeight"]));
    }

    cv::Mat blur1(blur0.size(), CV_8UC3); // first blur.
    cv::Mat blur2(blur1.size(), CV_8UC3); // second blur.
    std::vector<cv::Mat> b0planes, b1planes, b2planes;
    cv::Size blurSize;
    int difference = 0; // temporary holders for differences.
    double b0tob1 = 0,
           b1tob2 =
               0; // total of all distances between (b0 and b1) and (b1 and b2).

    // create blurs
    blurSize.height = blurSize.width =
        cv::min(cv::max(blur0.rows / 20, 3), 15); // adapt the blur size based
                                                  // on image dimensions but
                                                  // keep it within 3 and 15.
    cv::blur(blur0, blur1, blurSize);
    cv::blur(blur1, blur2, blurSize);

    // split into color planes
    split(blur0, b0planes);
    split(blur1, b1planes);
    split(blur2, b2planes);

    // calculate difference between blur0 and blur1.
    for (int y = 0; y < blur0.rows; y++) {
        for (int x = 0; x < blur0.cols; x++) {
            int Rdiff, Gdiff, Bdiff;

            Rdiff = Gdiff = Bdiff = 0;
            // distance in 3d space -> sqrt(xdiff^2+ydiff^2+zdiff^2)
            switch (b0planes.size()) {
            case 3:
                Rdiff = (int)b0planes[2].at<uchar>(y, x) -
                        (int)b1planes[2].at<uchar>(y, x);
            case 2:
                Gdiff = (int)b0planes[1].at<uchar>(y, x) -
                        (int)b1planes[1].at<uchar>(y, x);
            case 1:
                Bdiff = (int)b0planes[0].at<uchar>(y, x) -
                        (int)b1planes[0].at<uchar>(y, x);
            }
            b0tob1 =
                sqrt((double)(Bdiff * Bdiff + Gdiff * Gdiff + Rdiff * Rdiff));

            Rdiff = Gdiff = Bdiff = 0;
            switch (b1planes.size()) {
            case 3:
                Rdiff = (int)b1planes[2].at<uchar>(y, x) -
                        (int)b2planes[2].at<uchar>(y, x);
            case 2:
                Gdiff = (int)b1planes[1].at<uchar>(y, x) -
                        (int)b2planes[1].at<uchar>(y, x);
            case 1:
                Bdiff = (int)b1planes[0].at<uchar>(y, x) -
                        (int)b2planes[0].at<uchar>(y, x);
            }
            b1tob2 =
                sqrt((double)(Bdiff * Bdiff + Gdiff * Gdiff + Rdiff * Rdiff));

            int diff = abs(b0tob1 - b1tob2);
            if (diff > diffThreshold)
                difference += diff;
        }
    }

    if (useFaceRect && metrics["CvFrontalFaceFound"] > 0) {
        metrics["BlurFace"] = 1.0 * difference / (blur0.rows * blur0.cols);
    }
    else {
        metrics["Blur"] = 1.0 * difference / (blur0.rows * blur0.cols);
    }
}

void Face::setSAPLevel(std::map<std::string, double> &metrics)
{
    // Verify pre-conditions
    if (metrics["CvFrontalFaceFound"] <= 0) {
        metrics["SAPFailureCode"] = NO_FRONTAL_FACE_FOUND;
        return;
    }

    if (metrics["ImageWidth"] >= 3300 &&
        metrics["ImageHeight"] >= 4400) // level 50 and 51
    {
        bool sap50ImageRatio = (metrics["ImageRatio"] == (double)3 / 4);
        bool sap50FaceWidth =
            (metrics["CvFaceWidth"] >= metrics["ImageWidth"] * 0.5);
        bool sap51FaceWidth =
            (metrics["CvFaceWidth"] >= metrics["ImageWidth"] * 0.7);
        if (!sap50ImageRatio) {
            metrics["SAPFailureCode"] = LEVEL50_IMAGERATIO;
        }
        else if (!sap51FaceWidth) {
            // even though it does not meet all parts of sap level 51
            // it can still meet 50
            if (!sap50FaceWidth) {
                // sap level 50 failure
                metrics["SAPFailureCode"] = LEVEL50_FACEWIDTH;
            }
            else {
                metrics["SAPLevel"] = 50;
                // also sap level 51 failure
                metrics["SAPFailureCode"] = LEVEL51_FACEWIDTH;
            }
        }
        else {
            // must meet sap level 51
            metrics["SAPLevel"] = 51;
            metrics["SAPFailureCode"] = NO_FAILURE;
        }
    }
    else if (metrics["ImageWidth"] >= 768 &&
             metrics["ImageHeight"] >= 1024) // level 40
    {
        bool sap40ImageRatio = (metrics["ImageRatio"] == (double)3 / 4);
        bool sap40FaceWidth =
            (metrics["CvFaceWidth"] >= metrics["ImageWidth"] * 0.5);
        if (!sap40ImageRatio) {
            metrics["SAPFailureCode"] = LEVEL40_IMAGERATIO;
        }
        else if (!sap40FaceWidth) {
            metrics["SAPFailureCode"] = LEVEL40_FACEWIDTH;
        }
        else {
            // must meet sap level 40!
            metrics["SAPLevel"] = 40;
            metrics["SAPFailureCode"] = NO_FAILURE;
        }
    }
    else if (metrics["ImageWidth"] >= 480 &&
             metrics["ImageHeight"] >= 600) // level 30
    {
        bool sap30ImageRatio = (metrics["ImageRatio"] == (double)4 / 5);
        if (!sap30ImageRatio) {
            metrics["SAPFailureCode"] = LEVEL30_IMAGERATIO;
        }
        else {
            // must meet sap level 30!
            metrics["SAPLevel"] = 30;
            metrics["SAPFailureCode"] = NO_FAILURE;
        }
    }
    else {
        // DOES NOT MEET ANY SAP LEVEL 30 or above requirements
        metrics["SAPFailureCode"] = LEVEL30_RESOLUTION;
    }
}

void Face::setOpenBrMetrics(const cv::Mat &img,
                            std::map<std::string, double> &metrics)
{
    if (metrics["CvFrontalFaceFound"] != 1) {
        return;
    }

    // #ifdef USE_OPENBR
    // BrLandmarker::BrResult brResult;
    std::map<std::string, int> brResult;
    // use face rect found using the better aday trained cascade in the
    // cvlandmarker!  if no face found, could run the whole image
    QRectF faceRect(0, 0, img.cols, img.rows);
    if (metrics["CvFaceWidth"] != -1 && metrics["CvFaceHeight"] != -1) {
        faceRect = QRectF(metrics["CvFaceX"], metrics["CvFaceY"],
                          metrics["CvFaceWidth"], metrics["CvFaceHeight"]);
    }
    // the image passed in will be converted to gray by openbr
    brResult = brLandmarker.registerImage(img, faceRect, true, true);

    if (biqtMode != LANDMARK) {
        metrics["BrConfidence"] = brResult["confidence"];
        // capping it at 2500 then normalizing and inverting
        if (metrics["BrConfidence"] >= 2500) {
            metrics["BrConfidence"] = 2500;
        }

        // now normalize and invert if > 0
        if (metrics["BrConfidence"] > 0) {
            // TODO: Review.
            metrics["BrConfidence"] = 1 - (2500 - metrics["BrConfidence"]) / 2500;
        }
    }

    // also could output eye corners from flandmark through openbr if needed
    // right eye
    metrics["BrRightEyePosition_X"] = brResult["rightEye_x"];
    metrics["BrRightEyePosition_Y"] = brResult["rightEye_y"];
    // left eye
    metrics["BrLeftEyePosition_X"] = brResult["leftEye_x"];
    metrics["BrLeftEyePosition_Y"] = brResult["leftEye_y"];
    // set Br IPD
    if (biqtMode != LANDMARK) {
        metrics["BrIPD"] = calculateDistance(
            metrics["BrRightEyePosition_X"], metrics["BrRightEyePosition_Y"],
            metrics["BrLeftEyePosition_X"], metrics["BrLeftEyePosition_Y"]);
    }
    // #endif
}

// PUBLIC
void Face::prepMetricsWriteMapByMode(const FaceMode mode,
                                     std::map<std::string, double> &metrics)
{
    // first set all landmarking std::map keys since SHORT will contain
    // all of them and FULL will contain all of
    // using multiple ifs to keep the metrics in order
    metrics["ImageWidth"] = -1;
    metrics["ImageHeight"] = -1;

    if (mode == FULL) {
        metrics["ImageChannels"] = -1;
        metrics["ImageArea"] = -1;
        metrics["ImageRatio"] = -1;
    }

    metrics["CvFrontalFaceFound"] = 0;
    metrics["CvProfileFaceFound"] = 0;
    metrics["CvFaceX"] = -1;
    metrics["CvFaceY"] = -1;
    metrics["CvFaceWidth"] = -1;
    metrics["CvFaceHeight"] = -1;

    if (mode == FULL) {
        metrics["SAPLevel"] = 0;
        metrics["SAPFailureCode"] = NO_FAILURE;
        metrics["CvNumLandmarks"] = 0;
    }

    if (mode != LANDMARK)
        metrics["CvEyeCount"] = 0;

    metrics["CvRightEyePosition_X"] = -1;
    metrics["CvRightEyePosition_Y"] = -1;
    metrics["CvLeftEyePosition_X"] = -1;
    metrics["CvLeftEyePosition_Y"] = -1;

    if (mode != LANDMARK) {
        metrics["CvIPD"] = -1;
        metrics["CvNoseCount"] = 0;
    }

    metrics["CvNosePosition_X"] = -1;
    metrics["CvNosePosition_Y"] = -1;

    if (mode != LANDMARK)
        metrics["CvMouthCount"] = 0;

    metrics["CvMouthPosition_X"] = -1;
    metrics["CvMouthPosition_Y"] = -1;
    metrics["BrRightEyePosition_X"] = -1;
    metrics["BrRightEyePosition_Y"] = -1;
    metrics["BrLeftEyePosition_X"] = -1;
    metrics["BrLeftEyePosition_Y"] = -1;

    if (mode != LANDMARK) {
        metrics["BrIPD"] = 0;
    }

    if (mode == FULL) {
        metrics["FaceCenterOfMassX"] = -1;
        metrics["FaceCenterOfMassY"] = -1;
        metrics["FaceOffsetX"] = -1;
        metrics["FaceOffsetY"] = -1;
        metrics["SkinFull"] = 0;
    }

    if (mode != LANDMARK)
        metrics["SkinFace"] = 0;

    if (mode == FULL) {
        metrics["Focus"] = 0;
        metrics["FocusFace"] = 0;
        metrics["OverExposure"] = 0;
        metrics["OverExposureFace"] = 0;
        metrics["Blur"] = 0;
        metrics["BlurFace"] = 0;
        metrics["BGDeviation"] = 0;
        metrics["BGGrayness"] = 0;
    }
}

std::string Face::getSapFailureStr(int sapFailure)
{
    /*
    NO_FAILURE,
    NO_FRONTAL_FACE_FOUND,
    LEVEL30_RESOLUTION,
    LEVEL30_IMAGERATIO,
    LEVEL40_IMAGERATIO,
    LEVEL40_FACEWIDTH,
    LEVEL50_IMAGERATIO,
    LEVEL50_FACEWIDTH,
    LEVEL51_FACEWIDTH
    */

    if (sapFailure == 1)
        return "No frontal face found.";
    else if (sapFailure == 2)
        return "Image does not meet Level 30 Image Resolution.";
    else if (sapFailure == 3)
        return "Image does not meet Level 30 Image Ratio, but does meet the "
               "minimum resolution.";
    else if (sapFailure == 4)
        return "Image does not meet Level 40 Image Ratio, but does meet the "
               "minimum resolution.";
    else if (sapFailure == 5)
        return "Image does not meet Level 40 Face Width, but does meet the "
               "image ratio and minimum resolution.";
    else if (sapFailure == 6)
        return "Image does not meet Level 50 Image Ratio, but does meet the "
               "minimum resolution.";
    else if (sapFailure == 7)
        return "Image does not meet Level 50 Face Width, but does meet the "
               "image ratio and minimum resolution.";
    else if (sapFailure == 8)
        return "Image does not meet Level 51 Face Width, but does meet the "
               "image ratio and minimum resolution.";
    else
        return "No failure.";
}

double Face::getQuality(const std::vector<char> &img_data,
                        std::map<std::string, double> &metrics, FaceMode mode)
{
    cv::Mat img(imdecode(cv::Mat(img_data), CV_LOAD_IMAGE_COLOR));
    // doing a continuous check as well on the image
    if ((img.rows == 0) || (img.cols == 0) || img.data == NULL ||
        !img.isContinuous()) {
        return -1;
    }

    return getQuality(img, metrics, mode);
}

double Face::getQuality(const std::string image_path,
                        std::map<std::string, double> &metrics, FaceMode mode)
{
    cv::Mat img = cv::imread(image_path);
    // doing a continuous check as well on the image
    if ((img.rows == 0) || (img.cols == 0) || img.data == NULL ||
        !img.isContinuous()) {
        return -1;
    }

    return getQuality(img, metrics, mode);
}

double Face::getQuality(const cv::Mat &img,
                        std::map<std::string, double> &metrics, FaceMode mode,
                        const cv::Rect &detected_rect)
{
    // biqtMode can be used throughout the setters
    biqtMode = mode;

    /* Image Metrics */
    setWidth(img, metrics);
    setHeight(img, metrics);
    if (mode == FULL) {
        setChannels(img, metrics);
        setArea(img, metrics);
        setRatio(img, metrics);
    }

    CvLandmarker::LandmarkResult landmarkResult =
        cvLandmarker.getLandmarksNonThreaded(img, false, false, detected_rect);

    setFace(img, metrics, landmarkResult.landmarkFaces);
    // once we have set the face metrics we can get the SAP level
    if (mode == FULL) {
        setSAPLevel(metrics);
    }

    // only one face for now since the cv landmarker is getting the largest face
    if (landmarkResult.landmarkFaces.size() > 0) {
        setCvNumLandmarks(metrics, landmarkResult.landmarkFaces[0]);
        setEyeCount(img, metrics, landmarkResult.landmarkFaces[0]);
        setNoseCount(img, metrics, landmarkResult.landmarkFaces[0]);
        setMouthCount(img, metrics, landmarkResult.landmarkFaces[0]);
        // OpenBR
        setOpenBrMetrics(img, metrics);
    }
    // cv and br landmarks
    if (mode == LANDMARK) {
        return 0;
    }

    // run this in SHORT MODE to get determine skin
    setFaceOffset(img, metrics);

    if (mode == Face::FULL) {
        // the threshold aren't used, but could be to eliminate more images
        // likely to be FTE  best non-face threshold found at 19196.22
        setFocus(img, metrics, false);
        setFocus(img, metrics, true);
        // non-face threshold found at 0.7314
        setOverExposure(img, metrics, false);
        setOverExposure(img, metrics, true);
        // best non-face threshold found at 43.18254
        setBlur(img, metrics, false);
        setBlur(img, metrics, true);

        setBackground(img, metrics);
    }

    // coefficients were found using the InfoGainAtributeEval Ranker method
    // after normalizing the parameters  BrConfidence has already been
    // normalized from 0 to 1 and CvEyeCount will be divided by two to meet this
    // 0-1 requirement!

    // if no face found return 0
    if (metrics["CvFrontalFaceFound"] < 1) {
        return 0;
    }
    // SkinFace should only be -1 if no face found
    if (metrics["SkinFace"] < 0) {
        metrics["SkinFace"] = 0;
    }
    // getting the max for normalization from 0 to 1
    double overallQualityMax = 0.743 * 1 + 0.706 * (2 / 2) + 0.691 * 1.0 +
                               0.675 * 1 + 0.606 * 1 + 0.513 * 1;
    double overallQuality =
        ((0.743 * metrics["CvMouthCount"] +
          0.706 * (metrics["CvEyeCount"] / 2) + 0.691 * metrics["SkinFace"] +
          0.675 * metrics["CvFrontalFaceFound"] +
          0.606 * metrics["CvNoseCount"] + 0.513 * metrics["BrConfidence"]) /
         overallQualityMax);
    // NOTE: the best determined threshold using this score is 4.54, images
    // lower than this should be re-taken (recommendation)
    return 10 * overallQuality;
}
