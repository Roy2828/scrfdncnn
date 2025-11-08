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



#include "arcface/face_api.h"

#define LOG_TAG "FaceRecognition"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#if __ARM_NEON
#include <arm_neon.h>
#endif // __ARM_NEON



static FaceAPI* g_face_api = nullptr;
static bool g_initialized = false;

std::vector<std::vector<float>> convertJObjectArrayToVector(JNIEnv *pEnv, jobjectArray pArray);

extern "C" JNIEXPORT void JNICALL
Java_com_tencent_scrfdncnn_SCRFDNcnn_init(JNIEnv* env, jobject thiz,jstring scrfd_param_path,jstring scrfd_bin_path,jstring feat_param_path,jstring feat_bin_path) {
    if (g_initialized && g_face_api != nullptr) {
        LOGI("FaceAPI已经初始化，跳过重复初始化");
        return;
    }

    // 创建实例
    g_face_api = new FaceAPI();

    if (g_face_api) {

        const char* scrfdParamPath = env->GetStringUTFChars(scrfd_param_path, nullptr);
        const char* scrfdBinPath = env->GetStringUTFChars(scrfd_bin_path, nullptr);
        const char* featParamPath = env->GetStringUTFChars(feat_param_path, nullptr);
        const char* featBinPath = env->GetStringUTFChars(feat_bin_path, nullptr);

        bool success = g_face_api->init(false,scrfdParamPath,scrfdBinPath,featParamPath,featBinPath);
        if (success) {
            g_face_api->set_threshold(0.44f);
            g_initialized = true;
            LOGI("FaceAPI初始化成功");
        } else {
            LOGE("FaceAPI初始化失败");
            delete g_face_api;
            g_face_api = nullptr;
        }
    }
}



// 从图像获取嵌入向量
extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_tencent_scrfdncnn_SCRFDNcnn_getEmbeddingFromImage(
        JNIEnv* env,
        jobject thiz,
        jstring image_path) {

    const char* image_path_str = env->GetStringUTFChars(image_path, nullptr);

    auto embd1 = g_face_api-> get_embedding_from_image(image_path_str);
    env->ReleaseStringUTFChars(image_path, image_path_str);

    // 创建外层数组（数组的数组）
    jclass floatArrayClass = env->FindClass("[F");
    jobjectArray result = env->NewObjectArray(embd1.size(), floatArrayClass, nullptr);

    // 填充内层数组
    for (size_t i = 0; i < embd1.size(); i++) {
        jfloatArray innerArray = env->NewFloatArray(embd1[i].size());
        env->SetFloatArrayRegion(innerArray, 0, embd1[i].size(), embd1[i].data());
        env->SetObjectArrayElement(result, i, innerArray);
        env->DeleteLocalRef(innerArray);
    }

    return result;
}

//  比对嵌入向量与图像
extern "C" JNIEXPORT jfloat JNICALL
Java_com_tencent_scrfdncnn_SCRFDNcnn_compareEmbeddingWithImage(
        JNIEnv* env,
        jobject thiz,
        jobjectArray embd,  // float[][] 对应 std::vector<std::vector<float>>&
        jstring image_path) {

    if (embd == nullptr || image_path == nullptr) {
        LOGE("嵌入向量数组为空");
        return 0.0;
    }
    // 解析二维浮点数组
    jsize outer_len = env->GetArrayLength(embd);
    if (outer_len == 0) {
        LOGE("embedding 空");
        return 0.0;
    }

    std::vector<std::vector<float>> embedding = convertJObjectArrayToVector(env, embd);

    if (embedding.empty()) {
        LOGE("embedding 数据为空");
        return -1.0f;
    }

    const char* image_path_str = env->GetStringUTFChars(image_path, nullptr);
    float sim = g_face_api->compare_embedding_with_image(embedding, image_path_str);

    env->ReleaseStringUTFChars(image_path, image_path_str);
    return sim;
}




// 辅助函数：将 jobjectArray 转换为 std::vector<std::vector<float>>
std::vector<std::vector<float>> convertJObjectArrayToVector(JNIEnv *env, jobjectArray jarray) {
    std::vector<std::vector<float>> result;

    if (!jarray) {
        LOGE("Null jobjectArray provided");
        return result;
    }

    jsize outer_size = env->GetArrayLength(jarray);
    if (outer_size == 0) {
        LOGI("Empty outer array");
        return result;
    }

    result.reserve(outer_size);

    for (jsize i = 0; i < outer_size; i++) {
        jfloatArray inner_array = (jfloatArray)env->GetObjectArrayElement(jarray, i);
        if (!inner_array) {
            LOGE("Null inner array at index %d", i);
            result.push_back(std::vector<float>()); // 添加空向量
            continue;
        }

        jsize inner_size = env->GetArrayLength(inner_array);
        jfloat* inner_elements = env->GetFloatArrayElements(inner_array, nullptr);

        if (!inner_elements) {
            LOGE("Failed to get inner array elements at index %d", i);
            env->DeleteLocalRef(inner_array);
            result.push_back(std::vector<float>());
            continue;
        }

        std::vector<float> inner_vec(inner_size);
        for (jsize j = 0; j < inner_size; j++) {
            inner_vec[j] = inner_elements[j];
        }

        result.push_back(inner_vec);

        env->ReleaseFloatArrayElements(inner_array, inner_elements, JNI_ABORT);
        env->DeleteLocalRef(inner_array);
    }

    return result;
}




