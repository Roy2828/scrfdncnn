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


#include <string.h>
#include <benchmark.h>
#include "cpu.h"


#include "features.h"
#include "face_align.h"


using namespace std;
using namespace cv;
using namespace FacePreprocess;

int FEATURE::load(const std::string& param_path, const std::string& bin_path)
{
    feat.clear();

    ncnn::set_cpu_powersave(2);
    ncnn::set_omp_num_threads(ncnn::get_big_cpu_count());

    feat.opt = ncnn::Option();

#if NCNN_VULKAN
    feat.opt.use_vulkan_compute = use_gpu;
#endif

    feat.opt.num_threads = ncnn::get_big_cpu_count();

    // 直接使用传入路径加载模型
    if (feat.load_param(param_path.c_str()) != 0)
    {
        fprintf(stderr, "failed to load param file: %s\n", param_path.c_str());
        return -1;
    }

    if (feat.load_model(bin_path.c_str()) != 0)
    {
        fprintf(stderr, "failed to load bin file: %s\n", bin_path.c_str());
        return -1;
    }

    return 0;
}


int FEATURE::detect(const cv::Mat& rgb, const std::vector<FaceObject>& faceobjects, std::vector<std::vector<float> >& features)
{
    int width = rgb.cols;
    int height = rgb.rows;

    const float mean_vals[3] = {127.5f, 127.5f, 127.5f};
    const float norm_vals[3] = {1/127.5f, 1/127.5f, 1/127.5f};

    if (faceobjects.size()>0)
    {
        features.resize(faceobjects.size());
        //ncnn::Mat img = ncnn::Mat::from_pixels(rgb.data, ncnn::Mat::PIXEL_BGR2RGB, width, height);
        //std::vector<ncnn::Mat> align_faces;

        std::vector<cv::Mat> aligned_faces;
         
        double t_start = ncnn::get_current_time();
        //preprocess(img, faceobjects, align_faces);
        face_align(rgb, faceobjects, aligned_faces);
        printf("face align time %.02lfms\n", ncnn::get_current_time() - t_start);

        //cout<<"aligned_faces-num: "<<aligned_faces.size()<<endl;
        

        for(size_t i=0;i < aligned_faces.size(); i++)
        {
            //ncnn::Mat align_face = align_faces[i];
            //align_face.substract_mean_normalize(mean_vals, norm_vals);

            cv::Mat align_face = aligned_faces[i];
            ncnn::Mat in = ncnn::Mat::from_pixels(align_face.data, ncnn::Mat::PIXEL_BGR2RGB, align_face.cols, align_face.rows);
            in.substract_mean_normalize(mean_vals, norm_vals);
            
            ncnn::Extractor ex = feat.create_extractor();
            ex.set_light_mode(true);
            ex.input(0, in);
            
            ncnn::Mat out;
            ex.extract(177, out);
            //cout<<"out-h-w-c: "<<out.h<<", "<<out.w<<", "<<out.c<<endl;  // out-h-w-c: 1, 512, 1

            std::vector<float>feature;
            feature.resize(512);
            for (int j = 0; j < 512; j++){
                feature[j] = out[j];
                //cout<<"j-feat: "<<feature[j]<<endl;
                features[i] = feature;
            }    
        }
    }

    return 0;
}




