// #######################################################################
// NOTICE
//
// This software (or technical data) was produced for the U.S. Government
// under contract, and is subject to the Rights in Data-General Clause
// 52.227-14, Alt. IV (DEC 2007).
//
// Copyright 2019 The MITRE Corporation. All Rights Reserved.
// #######################################################################

#ifndef BRLANDMARKER_H
#define BRLANDMARKER_H

// #ifdef BRLANDMARKER_EXPORT
//     #ifdef _WIN32
//         #define BRLANDMARKER_LIBRARY __declspec(dllexport)
//     #else
//         #define BRLANDMARKER_LIBRARY
//         __attribute__((__visibility__("default")))
//     #endif
// #else
//     #ifdef _WIN32
//         #define BRLANDMARKER_LIBRARY __declspec(dllimport)
//     #else
//         #define BRLANDMARKER_LIBRARY
//     #endif
// #endif

// #ifdef USE_OPENBR
#ifdef _WIN32
#include "opencv2/core/core.hpp"
#include <QRectF>
#include <QString>
#endif
#include "openbr/openbr_plugin.h"

#include <iostream>

class BrLandmarker
// class BRLANDMARKER_LIBRARY BrLandmarker
{
  public:
    BrLandmarker();
    ~BrLandmarker();

    struct BrFace {
        bool containsLandmarks;
        bool fte;
        cv::Rect faceRect;
        cv::Point leftEye;
        cv::Point rightEye;
        cv::Point faceCenter;
        cv::Point rightCornerMouth;
        cv::Point leftCornerMouth;
        cv::Point nose;
        double DFFS; //(facenes or distance from face space, smaller is better)
    };

    void initialize(const std::string path);
    std::map<std::string, int> registerImage(const cv::Mat &img,
                                             const QRectF &faceRect,
                                             bool useASEF, bool forceDetection);
};

#endif // BRLANDMARKER_H
