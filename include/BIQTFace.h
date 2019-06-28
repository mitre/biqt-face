// #######################################################################
// NOTICE
//
// This software (or technical data) was produced for the U.S. Government
// under contract, and is subject to the Rights in Data-General Clause
// 52.227-14, Alt. IV (DEC 2007).
//
// Copyright 2019 The MITRE Corporation. All Rights Reserved.
// #######################################################################

#ifndef BIQTFACE_H
#define BIQTFACE_H

#include <fstream>
#include <json/json.h>
#include <json/value.h>

#include "Face.h"
#include "ProviderInterface.h"

class BIQTFace : public Provider {

  private:
    // Provider Object
    Face face;

  public:
    BIQTFace();
    ~BIQTFace() override;

    Provider::EvaluationResult evaluate(const std::string &file) override;
};

#endif
