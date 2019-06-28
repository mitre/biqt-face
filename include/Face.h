// #######################################################################
// NOTICE
//
// This software (or technical data) was produced for the U.S. Government
// under contract, and is subject to the Rights in Data-General Clause
// 52.227-14, Alt. IV (DEC 2007).
//
// Copyright 2019 The MITRE Corporation. All Rights Reserved.
// #######################################################################

#ifndef Face_H
#define Face_H

// #ifdef Face_EXPORT
//     #ifdef _WIN32
//         #define Face_LIBRARY __declspec(dllexport)
//     #else
//         #define Face_LIBRARY __attribute__((__visibility__("default")))
//     #endif
// #else
//     #ifdef _WIN32
//         #define Face_LIBRARY __declspec(dllimport)
//     #else
//         #define Face_LIBRARY
//     #endif
// #endif

#include "brlandmarker.h"
#include "cvlandmarker.h"
#include <map>
#include <string>
class Face
// class Face_LIBRARY Face
{
  public:
    void initializeMetricsWriteMap();
    Face();
    ~Face();

    enum FaceMode { FULL, SHORT, LANDMARK };

    enum SAPFailure {
        NO_FAILURE,
        NO_FRONTAL_FACE_FOUND,
        LEVEL30_RESOLUTION,
        LEVEL30_IMAGERATIO,
        LEVEL40_IMAGERATIO,
        LEVEL40_FACEWIDTH,
        LEVEL50_IMAGERATIO,
        LEVEL50_FACEWIDTH,
        LEVEL51_FACEWIDTH
    };

    // used to map enum values to a string representation
    std::map<int, std::string> metricsWriteMap;

    enum Metrics {
        // Image Metrics
        ImageWidth,
        ImageHeight,
        ImageChannels,
        ImageArea,
        ImageRatio,

        // Face Metrics
        CvFaceFound,
        CvFrontalFaceFound,
        CvProfileFaceFound,
        CvFaceX,
        CvFaceY,
        CvFaceWidth,
        CvFaceHeight,

        SAPLevel,
        SAPFailureCode,

        CvNumLandmarks,
        CvEyeCount,
        CvNoseCount,
        CvMouthCount,

        FaceCenterOfMassX,
        FaceCenterOfMassY,
        FaceOffsetX,
        FaceOffsetY,

        SkinFull,
        SkinFace,

        Focus,
        FocusFace,
        Blur,
        BlurFace,
        OverExposure,
        OverExposureFace,
        // OpenCV
        CvRightEyePosition_X,
        CvRightEyePosition_Y,
        CvLeftEyePosition_X,
        CvLeftEyePosition_Y,
        CvNosePosition_X,
        CvNosePosition_Y,
        CvMouthPosition_X,
        CvMouthPosition_Y,
        CvIPD,
        // OpenBr
        BrRightEyePosition_X,
        BrRightEyePosition_Y,
        BrLeftEyePosition_X,
        BrLeftEyePosition_Y,
        BrIPD,
        // background
        BGGrayness,
        BGDeviation
    };

    bool initialize(const std::string biqtPath);
    void finalize();
    void prepMetricsWriteMapByMode(const FaceMode mode,
                                   std::map<std::string, double> &metrics);
    // get the string result of the SAPFailure
    std::string getSapFailureStr(int sapFailure);
    double getQuality(const std::vector<char> &img_data,
                      std::map<std::string, double> &metrics, FaceMode mode);
    double getQuality(const std::string image_path,
                      std::map<std::string, double> &metrics, FaceMode mode);
    double getQuality(const cv::Mat &img,
                      std::map<std::string, double> &metrics, FaceMode mode,
                      const cv::Rect &detected_rect = cv::Rect(0, 0, 0, 0));

  private:
    FaceMode biqtMode;

    CvLandmarker cvLandmarker;
    BrLandmarker brLandmarker;

    void setOldQualityMetrics(const cv::Mat &img,
                              std::map<std::string, double> &metrics);
    double calculateDistance(int x1, int y1, int x2, int y2);
    void setWidth(const cv::Mat &img, std::map<std::string, double> &metrics);
    void setHeight(const cv::Mat &img, std::map<std::string, double> &metrics);
    void setChannels(const cv::Mat &img,
                     std::map<std::string, double> &metrics);
    void setArea(const cv::Mat &img, std::map<std::string, double> &metrics);
    void setRatio(const cv::Mat &img, std::map<std::string, double> &metrics);
    void setFace(const cv::Mat &img, std::map<std::string, double> &metrics,
                 const std::vector<CvLandmarker::LandmarkFace> &landmarkFaces);
    void setCvNumLandmarks(std::map<std::string, double> &metrics,
                           const CvLandmarker::LandmarkFace &landmarkFace);
    void setEyeCount(const cv::Mat &img, std::map<std::string, double> &metrics,
                     const CvLandmarker::LandmarkFace &landmarkFace);
    void setNoseCount(const cv::Mat &img,
                      std::map<std::string, double> &metrics,
                      const CvLandmarker::LandmarkFace &landmarkFace);
    void setMouthCount(const cv::Mat &img,
                       std::map<std::string, double> &metrics,
                       const CvLandmarker::LandmarkFace &landmarkFace);
    void getCircularROI(int R, std::vector<int> &RxV);
    void setOverExposure(const cv::Mat &img,
                         std::map<std::string, double> &metrics,
                         bool useFaceRect);
    void setFocus(const cv::Mat &img, std::map<std::string, double> &metrics,
                  bool useFaceRect);
    void setFaceOffset(const cv::Mat &img,
                       std::map<std::string, double> &metrics);
    void setBackground(const cv::Mat &img,
                       std::map<std::string, double> &metrics);
    void setBlur(const cv::Mat &img, std::map<std::string, double> &metrics,
                 bool useFaceRect);
    void setSAPLevel(std::map<std::string, double> &metrics);
    void setOpenBrMetrics(const cv::Mat &img,
                          std::map<std::string, double> &metrics);
    void setMetricsWriteMap(std::string name, int index);
};

#endif // Face_H
