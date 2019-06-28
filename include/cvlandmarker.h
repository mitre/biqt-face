// #######################################################################
// NOTICE
//
// This software (or technical data) was produced for the U.S. Government
// under contract, and is subject to the Rights in Data-General Clause
// 52.227-14, Alt. IV (DEC 2007).
//
// Copyright 2019 The MITRE Corporation. All Rights Reserved.
// #######################################################################

#ifndef CVLANDMARKER_H
#define CVLANDMARKER_H

// #ifdef CVLANDMARKER_EXPORT
//     #ifdef _WIN32
//         #define CVLANDMARKER_LIBRARY __declspec(dllexport)
//     #else
//         #define CVLANDMARKER_LIBRARY
//         __attribute__((__visibility__("default")))
//     #endif
// #else
//     #ifdef _WIN32
//         #define CVLANDMARKER_LIBRARY __declspec(dllimport)
//     #else
//         #define CVLANDMARKER_LIBRARY
//     #endif
// #endif

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/opencv.hpp"
#include <ctime>
#include <iostream>

class CvLandmarker
// class CVLANDMARKER_LIBRARY CvLandmarker
{
  public:
    CvLandmarker();
    ~CvLandmarker();

    bool initialize(std::string cascadesPath);

    struct LandmarkFace {
        bool containsLandmarks;
        bool isProfile;
        int numLandmarks;
        cv::Rect faceRect;
        // all Rects are based off of
        // faceRect coordinates
        // points are based off of the image
        cv::Rect eyePairRect;
        cv::Point leftEye;
        cv::Rect leftEyeRect;
        cv::Point rightEye;
        cv::Rect rightEyeRect;
        cv::Point noseTip;
        cv::Rect noseRect;
        cv::Point mouth;
        cv::Rect mouthRect;
    };

    struct LandmarkResult {
        std::string filePath;
        std::vector<LandmarkFace> landmarkFaces;
    };

    void checkRectOutOfBounds(const cv::Mat &img, cv::Rect &rect);
    // if no detected rect passed in the area will be zero and face detection
    // will be performed
    LandmarkResult getLandmarksNonThreaded(
        const cv::Mat &img, bool printLandmarks, bool showPreviews,
        const cv::Rect &detected_rect = cv::Rect(0, 0, 0, 0));

  private:
    cv::CascadeClassifier haarFaceCascade;
    cv::CascadeClassifier haarProfileFaceCascade;
    cv::CascadeClassifier lbpFaceCascade;
    cv::CascadeClassifier eyeCascade;
    cv::CascadeClassifier leftEyeCascade;
    cv::CascadeClassifier rightEyeCascade;
    cv::CascadeClassifier eyePairCascade;
    cv::CascadeClassifier noseCascade;
    cv::CascadeClassifier mouthCascade;

    std::string face_haar_cascade;         // haarcascade_frontalface_alt2.xml";
    std::string face_haar_profile_cascade; // haarcascade_profileface.xml";
    std::string face_lbp_cascade;          // cascade.xml"; -trained
    std::string eye_pair_cascade;          // haarcascade_mcs_eyepair_big.xml";
    std::string left_eye_cascade;          // haarcascade_mcs_lefteye.xml";
    std::string right_eye_cascade;         // haarcascade_mcs_righteye.xml";
    std::string
        nose_cascade; // = "cascades/haarcascades/haarcascade_mcs_nose.xml";
    std::string
        mouth_cascade; // = "cascades/haarcascades/haarcascade_mcs_mouth.xml";
};

#endif // CVLANDMARKER_H
