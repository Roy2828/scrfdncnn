// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2021 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#ifndef FEATURE_H
#define FEATURE_H

#include <opencv2/opencv.hpp>

#include <net.h>
#include "scrfd.h"




class FEATURE
{
public:
    int load(const std::string& param_path, const std::string& bin_path);
    bool use_gpu = false;
    int detect(const cv::Mat& rgb, const std::vector<FaceObject>& faceobjects, std::vector<std::vector<float> >& features);



private:
    ncnn::Net feat;
    

    //void getAffineMatrix(float* src_5pts, const float* dst_5pts, float* M);
    //void warpAffineMatrix(ncnn::Mat src, ncnn::Mat &dst, float *M, int dst_w, int dst_h);
    //int preprocess(ncnn::Mat img, const std::vector<FaceObject>& faceobjects, std::vector<ncnn::Mat>& align_faces);

};

#endif // FEATURE_H
