#include "face_api.h"
#include <iostream>
#include <string>


int main() {
    FaceAPI api;
    bool use_gpu = false; // 根据需要设置是否使用 GPU
    api.init(use_gpu);
    api.set_threshold(0.44f);

    const char* image_path = "/code/models/arcface/test_data/class_photo/het1404046813481591.jpg";
    cv::Mat img1 = cv::imread(image_path);
    cv::Mat img2 = cv::imread(image_path);

    // 测试接口：get_embedding_from_image
    auto embd1 = api.get_embedding_from_image(image_path);
    
    // 测试接口：compare_embedding_with_image
    float sim = api.compare_embedding_with_image(embd1, image_path);
    
    // float sim = api.compare_faces(img1, img2);

    std::cout << "Similarity = " << sim << std::endl;

    if (sim > 0.44f)
        std::cout << "→ Same person" << std::endl;
    else
        std::cout << "→ Different person" << std::endl;

    return 0;
}




























