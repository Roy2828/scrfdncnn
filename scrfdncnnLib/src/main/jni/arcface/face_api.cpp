#include "face_api.h"
#include "scrfd.h"
#include "features.h"
#include "utils.h"  
#include <iostream>

FaceAPI::FaceAPI() {}

FaceAPI::~FaceAPI() {}

bool FaceAPI::init(bool use_gpu, const std::string& scrfd_param_path, const std::string& scrfd_bin_path,
                   const std::string& feat_param_path, const std::string& feat_bin_path) {
    
    if (use_gpu)
    {
        detector_.use_gpu = use_gpu;
        feature_.use_gpu = use_gpu;
    }
    
    detector_.load(scrfd_param_path, scrfd_bin_path);
    feature_.load(feat_param_path, feat_bin_path);
    return true;
}

void FaceAPI::set_threshold(float t) {
    threshold_ = t;
}

std::vector<std::vector<float>> FaceAPI::extract_embedding(const cv::Mat& img) {
    std::vector<FaceObject> faceobjects;
    detector_.detect(img, faceobjects);
    std::vector<std::vector<float>> features;
    if (!faceobjects.empty())
    {
        feature_.detect(img, faceobjects, features);
    }
    return features;
}

// ✅ 新增函数：输入 image_path，输出该图像中所有人脸 embedding
std::vector<std::vector<float>> FaceAPI::get_embedding_from_image(const std::string& image_path)
{
    cv::Mat img = cv::imread(image_path);
    if (img.empty())
    {
        std::cerr << "Error: cannot read image " << image_path << std::endl;
        return {};
    }

    // 提取人脸 embedding
    return extract_embedding(img);
}


float FaceAPI::compare_embeddings(const std::vector<std::vector<float>>& embd1,
                                  const std::vector<std::vector<float>>& embd2)
{
    if (embd1.empty() || embd2.empty()) {
        std::cerr << "[compare_embeddings] Empty embedding input!" << std::endl;
        return 0.0f;
    }

    float best_sim = 0.f;
    for (auto &e1 : embd1) {
        for (auto &e2 : embd2) {
            float sim = cos_distance(e1, e2);
            if (sim > best_sim)
                best_sim = sim;
        }
    }

    return best_sim;
}

float FaceAPI::compare_faces(const cv::Mat& img1, const cv::Mat& img2)
{
    auto embd1 = extract_embedding(img1);
    auto embd2 = extract_embedding(img2);

    float sim = compare_embeddings(embd1, embd2);
    std::cout << "Max similarity = " << sim << std::endl;

    return sim;
}


float FaceAPI::compare_embedding_with_image(const std::vector<std::vector<float>>& embd,
                                            const std::string& image_path)
{
    cv::Mat img = cv::imread(image_path);
    if (img.empty())
    {
        std::cerr << "Error: cannot read image " << image_path << std::endl;
        return 0.f;
    }

    // 提取图片中的特征
    auto embd_img = extract_embedding(img);

    if (embd_img.empty())
    {
        std::cerr << "Warning: no face detected in " << image_path << std::endl;
        return 0.f;
    }

    // 调用已有 compare_embeddings 进行对比
    return compare_embeddings(embd, embd_img);
}
