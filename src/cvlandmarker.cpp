/// #######################################################################
// NOTICE
//
// This software (or technical data) was produced for the U.S. Government
// under contract, and is subject to the Rights in Data-General Clause
// 52.227-14, Alt. IV (DEC 2007).
//
// Copyright 2019 The MITRE Corporation. All Rights Reserved.
// #######################################################################

#include "cvlandmarker.h"
#include "opencv2/core.hpp"

CvLandmarker::CvLandmarker() {}

bool CvLandmarker::initialize(std::string biqtPath)
{
    std::string biqt_home = getenv("BIQT_HOME");
    // face
    face_haar_cascade = biqt_home + "/providers/BIQTFace/config/haarcascades/"
                                    "haarcascade_frontalface_alt2.xml";
    // NOTE - could use ear detector after profile detection or if frontal fails
    face_haar_profile_cascade =
        biqt_home +
        "/providers/BIQTFace/config/haarcascades/haarcascade_profileface.xml";
    face_lbp_cascade =
        biqt_home + "/providers/BIQTFace/config/lbpcascades/cascade.xml";
    // eyes
    eye_pair_cascade = biqt_home + "/providers/BIQTFace/config/haarcascades/"
                                   "haarcascade_mcs_eyepair_big.xml";
    left_eye_cascade =
        biqt_home +
        "/providers/BIQTFace/config/haarcascades/haarcascade_mcs_lefteye.xml";
    right_eye_cascade =
        biqt_home +
        "/providers/BIQTFace/config/haarcascades/haarcascade_mcs_righteye.xml";
    // nose
    nose_cascade =
        biqt_home +
        "/providers/BIQTFace/config/haarcascades/haarcascade_mcs_nose.xml";
    // mouth
    mouth_cascade =
        biqt_home +
        "/providers/BIQTFace/config/haarcascades/haarcascade_mcs_mouth.xml";

    // load cascades
    // FRONTAL FACE - HAAR
    if (!haarFaceCascade.load(face_haar_cascade)) {
        std::cerr << "Failed to load the haar face cascade from: "
                  << face_haar_cascade << std::endl;
        return false;
    }

    // PROFILE FACE - HAAR
    if (!haarProfileFaceCascade.load(face_haar_profile_cascade)) {
        std::cerr << "Failed to load the haar profile face cascade from: "
                  << face_haar_profile_cascade << std::endl;
        return false;
    }

    // FRONTAL FACE - LBP
    if (!lbpFaceCascade.load(face_lbp_cascade)) {
        std::cerr << "Failed to load the lbp face cascade from: "
                  << face_lbp_cascade << std::endl;
        return false;
    }

    // EYE PAIR - HAAR
    if (!eyePairCascade.load(eye_pair_cascade)) {
        std::cerr << "Failed to load the eye pair cascade from: "
                  << eye_pair_cascade << std::endl;
        return false;
    }

    // LEFT EYE - HAAR
    if (!leftEyeCascade.load(left_eye_cascade)) {
        std::cerr << "Failed to load the left eye cascade from: "
                  << left_eye_cascade << std::endl;
        return false;
    }

    // RIGHT EYE - HAAR
    if (!rightEyeCascade.load(right_eye_cascade)) {
        std::cerr << "Failed to load the right eye cascade from: "
                  << right_eye_cascade << std::endl;
        return false;
    }

    // NOSE - HAAR
    if (!noseCascade.load(nose_cascade)) {
        std::cerr << "Failed to load the nose cascade from: " << nose_cascade
                  << std::endl;
        return false;
    }

    // MOUTH - HAAR
    if (!mouthCascade.load(mouth_cascade)) {
        std::cerr << "Failed to load the mouth cascade from: " << mouth_cascade
                  << std::endl;
        return false;
    }

    return true;
}

CvLandmarker::~CvLandmarker() {}

void CvLandmarker::checkRectOutOfBounds(const cv::Mat &img, cv::Rect &rect) {}

CvLandmarker::LandmarkResult
CvLandmarker::getLandmarksNonThreaded(const cv::Mat &img, bool printLandmarks,
                                      bool showPreviews,
                                      const cv::Rect &detected_rect)
{
    double duration;
    clock_t start;
    if (printLandmarks) {
        start = clock();
    }

    // creating copy for showPreviews (cleaner than const_cast<>)
    cv::Mat imgPreview;
    if (showPreviews) {
        imgPreview = img;
    }
    cv::Mat imgGray = cv::Mat(img);
    std::vector<cv::Rect> facesFound;

    // convert the img to gray  and then equalize equalize
    if (img.channels() > 2) {
        cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
    }
    else {
        if (printLandmarks) {
            std::cerr << "1 channel image??" << std::endl;
        }
    }

    LandmarkResult landmarkResult;
    cv::Size minSize(64, 64);
    // equalizehist
    equalizeHist(imgGray, imgGray);
    if (imgGray.cols > 0) {
        if (detected_rect.area() == 0) {
            lbpFaceCascade.detectMultiScale(
                imgGray, facesFound, 1.1, 4, 0,
                minSize); //, CV_CASCADE_FIND_BIGGEST_OBJECT); //, minSize);
        }
        else {
            facesFound.push_back(detected_rect);
        }

        int largestArea = 0;
        int index = 0;
        cv::Rect largestRect;
        if (facesFound.size() > 1) {
            for (unsigned int i = 0; i < facesFound.size(); i++) {
                cv::Rect tempRect = facesFound[i];
                if (tempRect.area() > largestArea) {
                    largestArea = tempRect.area();
                    largestRect = tempRect;
                }
                index++;
            }

            if (largestArea > 0) {
                facesFound.clear();
                facesFound.push_back(largestRect);
            }
        }

        if (facesFound.size() > 0) {
            // using largest rect - there won't be more than one face
            int faceDetectionCount = 0;
            for (unsigned int i = 0; i < facesFound.size(); i++) {
                faceDetectionCount++;

                int eyePairDectionCount = 0;
                int leftEyeDetectionCount = 0;
                int rightEyeDetectionCount = 0;
                int noseDetectionCount = 0;
                int mouthDetectionCount = 0;

                LandmarkFace landmarkFace;
                landmarkFace.containsLandmarks = false;
                landmarkFace.isProfile = false;
                landmarkFace.numLandmarks = 0;
                // initialize all points to -1,-1
                // right eye
                landmarkFace.rightEye = cv::Point(-1, -1);
                // left eye
                landmarkFace.leftEye = cv::Point(-1, -1);
                // nose
                landmarkFace.noseTip = cv::Point(-1, -1);
                // mouth
                landmarkFace.mouth = cv::Point(-1, -1);

                // set face rect
                landmarkFace.faceRect = facesFound[i];

                int faceXStart = landmarkFace.faceRect.x;
                int faceYStart = landmarkFace.faceRect.y;
                int faceWidth = landmarkFace.faceRect.width;
                int faceHeight = landmarkFace.faceRect.height;

                std::vector<cv::Rect> eyesPair;
                std::vector<cv::Rect> eyesLeft;
                std::vector<cv::Rect> eyesRight;
                std::vector<cv::Rect> noses;
                std::vector<cv::Rect> mouths;

                // grayscale cropped face
                cv::Mat faceGray = imgGray(facesFound[i]);
                // creating copy of face for showPreview
                cv::Mat facePreview;
                if (showPreviews) {
                    facePreview = imgPreview(facesFound[i]);
                }

                //=========================================EYE
                //BOX====================================

                // start at faceStart and go down %60 - start at face x location
                // and only go to the width
                double eyeBoxStartRatio = 0.20;
                double eyeBoxHeightRatio = 0.40;
                cv::Rect upperRect(0, 0 + (faceHeight * eyeBoxStartRatio),
                                   faceWidth, (faceHeight * eyeBoxHeightRatio));

                if (showPreviews) {
                    rectangle(facePreview, upperRect, cv::Scalar(0, 255, 0), 2);
                    imshow("facePreview", facePreview);
                    cv::waitKey();
                }

                cv::Mat upperFaceGray = faceGray(upperRect);
                // eye pair detection
                eyePairCascade.detectMultiScale(upperFaceGray, eyesPair, 1.1, 4,
                                                cv::CASCADE_FIND_BIGGEST_OBJECT);

                // for the pair
                for (unsigned int j = 0; j < eyesPair.size(); j++) {
                    // this need to be based off the face coordinates
                    landmarkFace.eyePairRect = eyesPair[j];
                    landmarkFace.eyePairRect.y =
                        upperRect.y + landmarkFace.eyePairRect.y;

                    if (showPreviews) {
                        rectangle(facePreview, landmarkFace.eyePairRect,
                                  cv::Scalar(255, 0, 255), 3);
                        imshow("eyePairRect", facePreview);
                        cv::waitKey();
                    }

                    eyePairDectionCount++;
                }

                //=========================================LEFT
                //EYE====================================

                // create a left side region for the left eye cascade
                cv::Rect upperRectLeft;
                upperRectLeft.x = upperRect.x + upperRect.width / 2;
                upperRectLeft.y = upperRect.y;
                upperRectLeft.width = upperRect.width / 2;
                upperRectLeft.height = upperRect.height;

                cv::Mat upperFaceLeftGray = faceGray(upperRectLeft);

                // left
                leftEyeCascade.detectMultiScale(upperFaceLeftGray, eyesLeft,
                                                1.1, 4,
                                                cv::CASCADE_FIND_BIGGEST_OBJECT);

                for (unsigned int j = 0; j < eyesLeft.size(); j++) {
                    leftEyeDetectionCount++;

                    // must be based off of face coords
                    landmarkFace.leftEyeRect = eyesLeft[j];
                    landmarkFace.leftEyeRect.x =
                        upperRectLeft.x + landmarkFace.leftEyeRect.x;
                    landmarkFace.leftEyeRect.y =
                        upperRectLeft.y + landmarkFace.leftEyeRect.y;

                    if (showPreviews) {
                        rectangle(facePreview, landmarkFace.leftEyeRect,
                                  cv::Scalar(100, 0, 255), 2);
                        imshow("leftEyeRect", facePreview);
                        cv::waitKey();
                    }

                    // points needs to be based off the image!!
                    cv::Point center(faceXStart + landmarkFace.leftEyeRect.x +
                                         landmarkFace.leftEyeRect.width * 0.5,
                                     faceYStart + landmarkFace.leftEyeRect.y +
                                         landmarkFace.leftEyeRect.height * 0.5);
                    landmarkFace.leftEye = center;

                    if (showPreviews) {
                        circle(imgPreview, landmarkFace.leftEye, 2,
                               cv::Scalar(100, 0, 255), 2);
                        imshow("landmarkFace.leftEye", imgPreview);
                        cv::waitKey();
                    }
                }

                //=========================================RIGHT
                //EYE====================================

                // create a right side region for the right eye cascade
                cv::Rect upperRectRight;
                upperRectRight.x = upperRect.x;
                upperRectRight.y = upperRect.y;
                upperRectRight.width = upperRect.width / 2;
                upperRectRight.height = upperRect.height;

                cv::Mat upperFaceRightGray = faceGray(upperRectRight);

                // right
                rightEyeCascade.detectMultiScale(upperFaceRightGray, eyesRight,
                                                 1.1, 4,
                                                 cv::CASCADE_FIND_BIGGEST_OBJECT);

                for (unsigned int j = 0; j < eyesRight.size(); j++) {
                    rightEyeDetectionCount++;

                    // must be based off of face coords
                    landmarkFace.rightEyeRect = eyesRight[j];
                    landmarkFace.rightEyeRect.x =
                        upperRectRight.x + landmarkFace.rightEyeRect.x;
                    landmarkFace.rightEyeRect.y =
                        upperRectRight.y + landmarkFace.rightEyeRect.y;

                    if (showPreviews) {
                        rectangle(facePreview, landmarkFace.rightEyeRect,
                                  cv::Scalar(255, 0, 100), 2);
                        imshow("rightEyeRect", facePreview);
                        cv::waitKey();
                    }

                    // center must be based off of image coordinates
                    cv::Point center(faceXStart + landmarkFace.rightEyeRect.x +
                                         landmarkFace.rightEyeRect.width * 0.5,
                                     faceYStart + landmarkFace.rightEyeRect.y +
                                         landmarkFace.rightEyeRect.height *
                                             0.5);

                    landmarkFace.rightEye = center;

                    if (showPreviews) {
                        circle(imgPreview, landmarkFace.rightEye, 2,
                               cv::Scalar(255, 0, 100), 2);
                        imshow("landmarkFace.rightEye", imgPreview);
                        cv::waitKey();
                    }
                }

                //=========================================NOSE====================================

                double noseBoxStartRatio = 0.30;
                double noseBoxHeightRatio = 0.50;
                // implement same bottom face protection on the nose zone that
                // is on the mouth zone - could happen!  if the low point of the
                // rect is beyond the image length in the bring it back to the
                // bottom of the image!!!
                cv::Rect middleRect(0, 0 + (faceHeight * noseBoxStartRatio),
                                    faceWidth,
                                    (faceHeight * noseBoxHeightRatio));

                if (showPreviews) {
                    rectangle(facePreview, middleRect, cv::Scalar(255, 0, 0),
                              2);
                    imshow("middleRect", facePreview);
                    cv::waitKey();
                }

                cv::Mat middleFaceGray = faceGray(middleRect);

                // nose
                noseCascade.detectMultiScale(middleFaceGray, noses, 1.1, 4,
                                             cv::CASCADE_FIND_BIGGEST_OBJECT);

                for (unsigned int j = 0; j < noses.size(); j++) {
                    noseDetectionCount++;

                    // based off of face coordinates
                    landmarkFace.noseRect = noses[j];
                    landmarkFace.noseRect.x =
                        middleRect.x + landmarkFace.noseRect.x;
                    landmarkFace.noseRect.y =
                        middleRect.y + landmarkFace.noseRect.y;

                    if (showPreviews) {
                        rectangle(facePreview, landmarkFace.noseRect,
                                  cv::Scalar(0, 100, 255), 2);
                        imshow("noseRect", facePreview);
                        cv::waitKey();
                    }

                    // center must be based off of image coordinates
                    cv::Point center(faceXStart + landmarkFace.noseRect.x +
                                         landmarkFace.noseRect.width * 0.5,
                                     faceYStart + landmarkFace.noseRect.y +
                                         landmarkFace.noseRect.height * 0.5);

                    landmarkFace.noseTip = center;

                    if (showPreviews) {
                        circle(imgPreview, landmarkFace.noseTip, 2,
                               cv::Scalar(0, 100, 255), 2);
                        imshow("landmarkFace.noseTip", imgPreview);
                        cv::waitKey();
                    }
                }

                //=========================================MOUTH====================================

                // detect the mouth in the face
                // start try to extend 10% beyond the face rect, i believe some
                // information is lost  NOTE: if the low point of the rect is
                // beyond the image length in the bring it back to the bottom of
                // the image!!!
                double mouthBoxStartRatio = 0.60;
                double mouthBoxHeightRatio = 0.45;
                cv::Rect lowerRect(0, 0 + (faceHeight * mouthBoxStartRatio),
                                   faceWidth,
                                   0 + (faceHeight * mouthBoxHeightRatio));
                int lowerRectBottom = faceYStart + faceHeight * 1.05;
                if (lowerRectBottom > img.rows) {
                    lowerRect.height =
                        lowerRect.height - (lowerRectBottom - img.rows);
                }

                // need a lower rect with image base since the lower rect
                // extends beyond the face rect
                cv::Rect lowerRectImg = lowerRect;
                lowerRectImg.x = faceXStart + lowerRect.x;
                lowerRectImg.y = faceYStart + lowerRect.y;
                if (showPreviews) {
                    // have to show on the full image since it goes below the
                    // mouth
                    rectangle(imgPreview, lowerRectImg, cv::Scalar(0, 0, 255),
                              2);
                    imshow("lowerRectDisp", imgPreview);
                    cv::waitKey();
                }

                cv::Mat lowerFaceGray = imgGray(lowerRectImg);

                // mouth
                mouthCascade.detectMultiScale(
                    lowerFaceGray, mouths, 1.1, 4,
                    cv::CASCADE_FIND_BIGGEST_OBJECT); //, cv::Size(40, 40) );

                for (unsigned int j = 0; j < mouths.size(); j++) {
                    mouthDetectionCount++;

                    // based off of face coordinates
                    landmarkFace.mouthRect = mouths[j];
                    landmarkFace.mouthRect.x =
                        lowerRect.x + landmarkFace.mouthRect.x;
                    landmarkFace.mouthRect.y =
                        lowerRect.y + landmarkFace.mouthRect.y;

                    if (showPreviews) {
                        rectangle(facePreview, landmarkFace.mouthRect,
                                  cv::Scalar(100, 255, 100), 2);
                        imshow("mouthRect", facePreview);
                        cv::waitKey();
                    }

                    // center must be based off of image coordinates
                    cv::Point center(faceXStart + landmarkFace.mouthRect.x +
                                         landmarkFace.mouthRect.width * 0.5,
                                     faceYStart + landmarkFace.mouthRect.y +
                                         landmarkFace.mouthRect.height * 0.5);

                    landmarkFace.mouth = center;

                    if (showPreviews) {
                        circle(imgPreview, landmarkFace.mouth, 2,
                               cv::Scalar(100, 255, 100), 2);
                        imshow("landmarkFace.mouth", imgPreview);
                        cv::waitKey();
                    }
                }

                landmarkFace.numLandmarks =
                    leftEyeDetectionCount + rightEyeDetectionCount +
                    noseDetectionCount + mouthDetectionCount;
                if (landmarkFace.numLandmarks > 0) {
                    landmarkFace.containsLandmarks = true;
                }
                // push back the result
                landmarkResult.landmarkFaces.push_back(landmarkFace);
            }
        }
        else {
            LandmarkFace landmarkFace;
            landmarkFace.containsLandmarks = false;
            landmarkFace.isProfile = false;
            landmarkFace.numLandmarks = 0;

            std::vector<cv::Rect> profileFaces;
            // try to see if there is a profile face!
            haarProfileFaceCascade.detectMultiScale(
                imgGray, profileFaces, 1.1, 4, cv::CASCADE_FIND_BIGGEST_OBJECT,
                minSize);
            if (profileFaces.size() > 0) {
                landmarkFace.isProfile = true;
                // searching for largest object - will be only one face
                landmarkFace.faceRect = profileFaces[0];

                // push back the result if faces are found!
                landmarkResult.landmarkFaces.push_back(landmarkFace);
            }
        }
    }

    if (printLandmarks && landmarkResult.landmarkFaces.size() > 0) {
        LandmarkFace landmarkFace = landmarkResult.landmarkFaces[0];
        std::cerr << "Right Eye X: " << landmarkFace.rightEye.x
                  << " Y: " << landmarkFace.rightEye.y << std::endl;
        std::cerr << "Left Eye X: " << landmarkFace.leftEye.x
                  << " Y: " << landmarkFace.leftEye.y << std::endl;
        std::cerr << "Nose X: " << landmarkFace.noseTip.x
                  << " Y: " << landmarkFace.noseTip.y << std::endl;
        std::cerr << "Mouth X: " << landmarkFace.mouth.x
                  << " Y: " << landmarkFace.mouth.y << std::endl;

        duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
        std::cerr << "Processed in: " << duration << " seconds" << std::endl;
    }

    return landmarkResult;
}
