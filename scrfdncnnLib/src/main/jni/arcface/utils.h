#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

// 依赖的外部类型
#include "scrfd.h"
#include "features.h"

// // 定义 FaceObject 结构（如果未在别处定义）
// // 如果已在 SCRFD.h 中定义，就可以去掉这一段。
// #ifndef FACE_OBJECT_DEFINED
// #define FACE_OBJECT_DEFINED
// struct FaceObject {
//     cv::Rect_<float> rect;
//     float prob;
// };
// #endif

// 计算两个特征向量的余弦相似度
float cos_distance(const std::vector<float>& a, const std::vector<float>& b);

// 对单张图片进行人脸检测与特征提取
// std::vector<std::vector<float>> test_image(cv::Mat& img, SCRFD& g_scrfd, FEATURE& g_feat);
std::vector<std::vector<float>> test_image(const cv::Mat& img, SCRFD& g_scrfd, FEATURE& g_feat);


// 批量处理目录中的图片（写入文件）
void test_imgdir(SCRFD& g_scrfd, FEATURE& g_feat);

#endif // UTILS_H
