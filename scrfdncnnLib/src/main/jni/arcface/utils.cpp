#include "utils.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <benchmark.h>


float cos_distance(const std::vector<float>& a, const std::vector<float>& b)
{
    if (a.size() != b.size() || a.empty() || b.empty())
        return -1.f;

    float reg_norm = 0.f, ver_norm = 0.f, sum = 0.f;

    for (size_t i = 0; i < a.size(); i++)
    {
        sum += a[i] * b[i];
        reg_norm += a[i] * a[i];
        ver_norm += b[i] * b[i];
    }

    float denom = std::sqrt(reg_norm) * std::sqrt(ver_norm);
    if (denom <= 1e-6f)
        return 0.f;

    return std::fabs(sum / denom);
}


void test_imgdir(SCRFD &g_scrfd, FEATURE &g_feat)
{
    std::vector<cv::String> imagelist;
    // ⚠️ 根据实际路径修改
    cv::String folder = "/home/yexiaofeng/program/Blaze_face/ncnn-pc-scrfd/query_face";

    cv::glob(folder, imagelist);
    int total_num = 0;
    std::ofstream ofs("query_faces.txt", std::ios::out | std::ios::app);

    double t_start = ncnn::get_current_time();
    for (size_t i = 0; i < imagelist.size(); ++i)
    {
        cv::Mat img = cv::imread(imagelist[i]);
        if (img.empty())
        {
            std::cerr << "Failed to load image: " << imagelist[i] << std::endl;
            continue;
        }

        std::vector<FaceObject> faceobjects;
        g_scrfd.detect(img, faceobjects);
        total_num += static_cast<int>(faceobjects.size());

        std::vector<std::vector<float>> features;
        if (!faceobjects.empty())
        {
            g_feat.detect(img, faceobjects, features);
            ofs << imagelist[i] << "\t";

            for (size_t k = 0; k < faceobjects.size(); ++k)
            {
                ofs << faceobjects[k].rect.x << " "
                    << faceobjects[k].rect.y << " "
                    << faceobjects[k].rect.x + faceobjects[k].rect.width << " "
                    << faceobjects[k].rect.y + faceobjects[k].rect.height << "\t";

                for (size_t n = 0; n < features[k].size(); n++)
                    ofs << features[k][n] << " ";

                ofs << "\t";
            }
            ofs << "\n";
        }
    }

    ofs.close();
    printf("total time %.03lfms\n", ncnn::get_current_time() - t_start);
    std::cout << "total_num: " << total_num << std::endl;
}
