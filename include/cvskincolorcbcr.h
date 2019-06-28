// #######################################################################
// NOTICE
//
// This software (or technical data) was produced for the U.S. Government
// under contract, and is subject to the Rights in Data-General Clause
// 52.227-14, Alt. IV (DEC 2007).
//
// Copyright 2019 The MITRE Corporation. All Rights Reserved.
// #######################################################################

#ifndef CV_SKINCOLOR_CBCR_INCLUDED
#define CV_SKINCOLOR_CBCR_INCLUDED

#include <opencv2/core/core.hpp>

/**
 * Skin Color Detection in (Cb, Cr) space by [1][2]
 *
 * @param img  Input image
 * @param mask Generated mask image. 1 for skin and 0 for others
 *
 * References)
 * @verbatim
 *  [1] R.L. Hsu, M. Abdel-Mottaleb, A.K. Jain, "Face Detection in Color
 * Images," IEEE Transactions on Pattern Analysis and Machine Intelligence ,vol.
 * 24, no. 5, pp. 696-706, May, 2002. (Original) [2] P. Peer, J. Kovac, J. and
 * F. Solina, ”Human skin colour clustering for face detection”, In: submitted
 * to EUROCON – International Conference on Computer as a Tool , 2003. (Tuned)
 * @endverbatim
 */

void cvSkinColorCrCb(const cv::Mat &img, cv::Mat &mask);

#endif
