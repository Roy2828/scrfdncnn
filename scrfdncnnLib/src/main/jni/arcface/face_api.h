#ifndef FACE_API_H
#define FACE_API_H

#include <opencv2/opencv.hpp>
#include <vector>
#include "scrfd.h"
#include "features.h"

class FaceAPI {
public:
    FaceAPI();
    ~FaceAPI();

    // 初始化检测器与特征提取模型
    bool init(bool use_gpu = false, 
              const std::string& scrfd_param_path = "../models/detect.param",
              const std::string& scrfd_bin_path = "../models/detect.bin",
              const std::string& feat_param_path = "../models/feature.param",
              const std::string& feat_bin_path = "../models/feature.bin");

    // 提取单张图片的所有人脸特征
    std::vector<std::vector<float>> extract_embedding(const cv::Mat& img);

    // 计算两张图片是否为同一人
    // 返回最大相似度值
    float compare_faces(const cv::Mat& img1, const cv::Mat& img2);

    float compare_embeddings(const std::vector<std::vector<float>>& embd1, const std::vector<std::vector<float>>& embd2);

    // ✅ 新增接口：输入特征 + 图片路径
    float compare_embedding_with_image(const std::vector<std::vector<float>>& embd,
                                       const std::string& image_path);
    std::vector<std::vector<float>> get_embedding_from_image(const std::string& image_path);

    // 设置阈值
    void set_threshold(float t);

private:
    float threshold_ = 0.44f;
    SCRFD detector_;
    FEATURE feature_;
};

#endif // FACE_API_H
