// #######################################################################
// NOTICE
//
// This software (or technical data) was produced for the U.S. Government
// under contract, and is subject to the Rights in Data-General Clause
// 52.227-14, Alt. IV (DEC 2007).
//
// Copyright 2019 The MITRE Corporation. All Rights Reserved.
// #######################################################################

#include "brlandmarker.h"

BrLandmarker::BrLandmarker() {}

BrLandmarker::~BrLandmarker() { br::Context::finalize(); }

// kept as void - nothing returned from br initialize
void BrLandmarker::initialize(const std::string path)
{
    // stackoverflow fix, gcc is fine with char* args[0], but not visual c++
    // setting the size to 1 will probably throw an issue somewhere - TODO:
    // verify it does not  const int i=1; // OK the size is const now!
    char *args[1]; // msvc forces this to be 1 or greater, but doesn't seem to
                   // cause an issue during runtime
    // char* args[0];
    std::string biqt_home = getenv("BIQT_HOME");
    QString sdkPath =
        QString::fromStdString(biqt_home + "/providers/BIQTFace/config/");
    std::cerr << "OpenBR sdk path: " << sdkPath.toStdString().c_str()
              << std::endl;
    std::cerr << "Initializing OpenBR..." << std::endl;
    int argc = 0;
    br::Context::initialize(argc, args, sdkPath, true);
    std::cerr << "Done initializing OpenBR" << std::endl;
}

/*
 * passing in the faceRect
 */
std::map<std::string, int> BrLandmarker::registerImage(const cv::Mat &img,
                                                       const QRectF &faceRect,
                                                       bool useASEF,
                                                       bool forceDetection)
{
    // Initialize transforms
    QSharedPointer<br::Transform> transform =
        br::Transform::fromAlgorithm("FaceRecognition");
    // QSharedPointer<br::Transform> transform2 =
    // br::Transform::fromAlgorithm("FaceQuality");

    // Initialize templates
    br::Template brTemplate;
    brTemplate.append(img);
    brTemplate.file.appendRect(faceRect);

    // Enroll templates
    brTemplate >> *transform;
    // brTemplate >> *transform2;

    std::map<std::string, int> result;
    result["rightEye_x"] = brTemplate.file.get<QPoint>("StasmRightEye").x();
    result["rightEye_y"] = brTemplate.file.get<QPoint>("StasmRightEye").y();
    result["leftEye_x"] = brTemplate.file.get<QPoint>("StasmLeftEye").x();
    result["leftEye_y"] = brTemplate.file.get<QPoint>("StasmLeftEye").y();
    result["confidence"] = brTemplate.file.get<int>("Confidence");

    return result;
}

/////////////////
///////////////////////

//     br::Template brTemplate;
//     //append the image and add set the ROI
//     brTemplate.append(img);
//     brTemplate.file.appendRect(faceRect);

//     //don't want to see the ouput
//     br::Globals->set_quiet(true);
//     std::cerr << "1" << std::endl;
//     br::Transform *trans = 0;
//     if(useASEF)
//     {
//         std::cerr << "a" << std::endl;
//         br::Globals->abbreviations.insert("FaceRecognitionRegistration","(ASEFEyes+Affine(88,88,0.25,0.35)+FTE(DFFS,instances=1))");
//     }
//     else
//     {
//         std::cerr << "b" << std::endl;
//         br::Globals->abbreviations.insert("FaceRecognitionRegistration","(Flandmark+Affine(88,88,0.25,0.35)+FTE(DFFS,instances=1))");
//     }
//     //converting image to gray - it will come uncoverted (as const image)
//     even though the ROI (face rect) has been found trans =
//     br::Transform::make("Cvt(Gray)+<FaceRecognitionRegistration>", NULL);
//     std::cerr << "c" << std::endl;
//     //project transform
//     br::Template dstTemplate;
//     trans->project(brTemplate, dstTemplate);

//     bool brSuccess = false;
//     //check first item to see if there was successful registration
//     if(dstTemplate.file.contains("FTE"))
//     {
//         brSuccess = true;
//     }

//     BrFace brFace;
//     //initialize variables
//     brFace.containsLandmarks = false;
//     brFace.fte = false;
//     brFace.DFFS = 0.0;

//     if(brSuccess)
//     {
//         //get FTE result from openbr
//         //already checked to see if it is contained
//         brFace.fte = dstTemplate.file.getBool("FTE");

//         //try to construct face rect
//         //should always contain ROIs if contains FTE
//         //this is currently repeating work done since this -
//         brTemplate.file.appendROI(faceRect) is used
//         if(!dstTemplate.file.rects().empty())
//         {
//             QList<QRectF> faceRois = dstTemplate.file.rects();
//             if(faceRois.size() > 0)
//             {
//                 QRectF faceRect = faceRois.at(0);

//                 brFace.faceRect.x = faceRect.x();
//                 brFace.faceRect.y = faceRect.y();
//                 brFace.faceRect.width = faceRect.width();
//                 brFace.faceRect.height = faceRect.height();
//             }
//         }

//         //should always contain both eyes if successful registration - but
//         checking the x on both eyes just in case
//         if(dstTemplate.file.contains("Affine_0_X") &&
//         dstTemplate.file.contains("Affine_1_X"))
//         {
//             brFace.containsLandmarks = true;
//             // File::get<float, File>(QList<File>() << f1 << f2, "Key1");
//             brFace.rightEye.x = dstTemplate.file.contains("Affine_0_X") ?
//             dstTemplate.file.value("Affine_0_X").toInt() : -1;
//             brFace.rightEye.y = dstTemplate.file.contains("Affine_0_Y") ?
//             dstTemplate.file.value("Affine_0_Y").toInt() : -1;
//             brFace.leftEye.x = dstTemplate.file.contains("Affine_1_X") ?
//             dstTemplate.file.value("Affine_1_X").toInt() : -1;
//             brFace.leftEye.y = dstTemplate.file.contains("Affine_1_Y") ?
//             dstTemplate.file.value("Affine_1_Y").toInt() : -1;
//             brFace.faceCenter.x =
//             dstTemplate.file.contains("FL_FaceCenter_X") ?
//             dstTemplate.file.value("FL_FaceCenter_X").toInt() : -1;
//             brFace.faceCenter.y =
//             dstTemplate.file.contains("FL_FaceCenter_Y") ?
//             dstTemplate.file.value("FL_FaceCenter_Y").toInt() : -1;
//             brFace.rightCornerMouth.x =
//             dstTemplate.file.contains("FL_RightMouthCorner_X") ?
//             dstTemplate.file.value("FL_RightMouthCorner_X").toInt() : -1;
//             brFace.rightCornerMouth.y =
//             dstTemplate.file.contains("FL_RightMouthCorner_Y") ?
//             dstTemplate.file.value("FL_RightMouthCorner_Y").toInt() : -1;
//             brFace.leftCornerMouth.x =
//             dstTemplate.file.contains("FL_LeftMouthCorner_X") ?
//             dstTemplate.file.value("FL_LeftMouthCorner_X").toInt() : -1;
//             brFace.leftCornerMouth.y =
//             dstTemplate.file.contains("FL_LeftMouthCorner_Y") ?
//             dstTemplate.file.value("FL_LeftMouthCorner_Y").toInt() : -1;
//             brFace.nose.x = dstTemplate.file.contains("FL_Nose_X") ?
//             dstTemplate.file.value("FL_Nose_X").toInt() : -1; brFace.nose.y =
//             dstTemplate.file.contains("FL_Nose_Y") ?
//             dstTemplate.file.value("FL_Nose_Y").toInt() : -1;
//         }

//         //should always contain DFFS, same deal as eyes above (ONLY WITH
//         ASEF) if(dstTemplate.file.contains("DFFS"))
//         {
//             brFace.DFFS =  dstTemplate.file.value("DFFS").toDouble();
//         }

//         //important
//         delete trans;
//     }

//     //only looking at one face using the passed in faceRect
//     BrResult brResult;
//     brResult.brFaces.push_back(brFace);
//     return brResult;
// }
