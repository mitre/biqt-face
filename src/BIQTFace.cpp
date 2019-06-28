// #######################################################################
// NOTICE
//
// This software (or technical data) was produced for the U.S. Government
// under contract, and is subject to the Rights in Data-General Clause
// 52.227-14, Alt. IV (DEC 2007).
//
// Copyright 2019 The MITRE Corporation. All Rights Reserved.
// #######################################################################

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include "BIQTFace.h"

/**
 *  Creates a BIQTFace instance
 */
BIQTFace::BIQTFace()
{
    // Initialize metadata
    std::string biqt_home = getenv("BIQT_HOME");
    std::ifstream desc_file(biqt_home + "/providers/BIQTFace/descriptor.json",
                            std::ifstream::binary);
    desc_file >> DescriptorObject;

    // Initialize module
    face.initialize("");
}

/**
 * The destructor.
 */
BIQTFace::~BIQTFace() { face.finalize(); }

/**
 * Evaluates the face images.
 *
 * @param file the input file.
 *
 * @return The result of the evaluation.
 */
Provider::EvaluationResult BIQTFace::evaluate(const std::string &file)
{
    // Initialize some variables
    Face::FaceMode mode = Face::FULL;
    Provider::EvaluationResult eval_result;
    Provider::QualityResult quality_result;

    // Read input file
    std::map<std::string, double> module_result;
    double quality = face.getQuality(file, module_result, mode);

    // If there was an error reading the image
    if (quality == -1) {
        eval_result.errorCode = 1;
        return eval_result;
    }

    // Construct evaluation result
    eval_result.errorCode = 0;
    quality_result.metrics["quality"] = quality;
    
    quality_result.metrics["background_deviation"] = module_result["BGDeviation"];
    quality_result.metrics["background_grayness"] = module_result["BGGrayness"];
    quality_result.metrics["blur"] = module_result["Blur"];
    quality_result.metrics["blur_face"] = module_result["BlurFace"];
    
    quality_result.metrics["focus"] = module_result["Focus"];
    quality_result.metrics["focus_face"] = module_result["FocusFace"];
    
    quality_result.metrics["openbr_confidence"] = module_result["BrConfidence"];
    quality_result.metrics["openbr_IPD"] = module_result["BrIPD"];
    
    quality_result.metrics["opencv_face_found"] = module_result["CvFaceFound"];
    quality_result.metrics["opencv_frontal_face_found"] = module_result["CvFrontalFaceFound"];
    quality_result.metrics["opencv_profile_face_found"] = module_result["CvProfileFaceFound"];
    
    quality_result.metrics["opencv_face_height"] = module_result["CvFaceHeight"];
    quality_result.metrics["opencv_face_width"] = module_result["CvFaceWidth"];    
    quality_result.metrics["opencv_IPD"] = module_result["CvIPD"];
    quality_result.metrics["opencv_landmarks_count"] = module_result["CvNumLandmarks"];    
    quality_result.metrics["opencv_eye_count"] = module_result["CvEyeCount"];
    quality_result.metrics["opencv_mouth_count"] = module_result["CvMouthCount"];    
    quality_result.metrics["opencv_nose_count"] = module_result["CvNoseCount"];
    
    quality_result.metrics["over_exposure"] = module_result["OverExposure"];
    quality_result.metrics["over_exposure_face"] = module_result["OverExposureFace"];
    
    quality_result.metrics["sap_code"] = module_result["SAPFailureCode"];
    quality_result.metrics["skin_ratio_face"] = module_result["SkinFace"];
    quality_result.metrics["skin_ratio_full"] = module_result["SkinFull"];

    // Features std::map
    
    // Image
    quality_result.features["image_area"] = module_result["ImageArea"];    
    quality_result.features["image_channels"] = module_result["ImageChannels"];
    quality_result.features["image_height"] = module_result["ImageHeight"];
    quality_result.features["image_ratio"] = module_result["ImageRatio"];
    quality_result.features["image_width"] = module_result["ImageWidth"];
    
    // OpenCV :: Face
    
    quality_result.features["opencv_face_x"] = module_result["CvFaceX"];
    quality_result.features["opencv_face_y"] = module_result["CvFaceY"];    
    quality_result.features["opencv_face_center_of_mass_x"] = module_result["FaceCenterOfMassX"];
    quality_result.features["opencv_face_center_of_mass_y"] = module_result["FaceCenterOfMassY"];
    quality_result.features["opencv_face_offset_x"] = module_result["FaceOffsetX"];
    quality_result.features["opencv_face_offset_y"] = module_result["FaceOffsetY"];
    
    // OpenCV :: Nose
    
    quality_result.features["opencv_nose_x"] = module_result["CvNosePosition_X"];
    quality_result.features["opencv_nose_y"] = module_result["CvNosePosition_Y"];
    
    // OpenCV :: Eyes
    
    quality_result.features["opencv_left_eye_x"] = module_result["CvLeftEyePosition_X"];
    quality_result.features["opencv_left_eye_y"] = module_result["CvLeftEyePosition_Y"];
    quality_result.features["opencv_right_eye_x"] = module_result["CvRightEyePosition_X"];
    quality_result.features["opencv_right_eye_y"] = module_result["CvRightEyePosition_Y"];    
    
    // OpenCV :: Mouth
    
    quality_result.features["opencv_mouth_x"] = module_result["CvMouthPosition_X"];
    quality_result.features["opencv_mouth_y"] = module_result["CvMouthPosition_Y"];
    
    // OpenBR
    
    quality_result.features["openbr_left_eye_x"] = module_result["BrLeftEyePosition_X"];
    quality_result.features["openbr_left_eye_y"] = module_result["BrLeftEyePosition_Y"];
    quality_result.features["openbr_right_eye_x"] = module_result["BrRightEyePosition_X"];
    quality_result.features["openbr_right_eye_y"] = module_result["BrRightEyePosition_Y"];
    
    eval_result.qualityResult.push_back(std::move(quality_result));

    return eval_result;
}

/**
 * Runs BIQTFace with the given parameters.
 *
 * @param cFilePath The path to the input file.
 *
 * @return the result status.
 */
DLL_EXPORT const char *provider_eval(const char *cFilePath)
{
    BIQTFace p;

    // calculate result
    std::string filePath(cFilePath);
    Provider::EvaluationResult result = p.evaluate(filePath);
    return Provider::serializeResult(result);
}

#ifdef FACE_MAKE_EXEC
#include <libgen.h>

int main(int argc, char **argv)
{
    // parse arguments
    std::string filePath = argv[1];
    std::string outputType = argv[2];
    int isFileList = atoi(argv[3]);

    setenv("BIQT_HOME", dirname(argv[0]), 0);
    provider_eval(filePath);
}
#endif
