#include <jni.h>
#include <string>
#include <android/log.h>
#include <thread>
#include <chrono>

#define LOG_TAG "JNIDemo"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// 全局引用，用于缓存类和方法的ID
static jclass resultDataClass = nullptr;
static jclass userInfoClass = nullptr;
static jclass callbackInterfaceClass = nullptr;

// ResultData 构造函数和方法ID
static jmethodID resultDataConstructor = nullptr;
static jmethodID resultDataGetId = nullptr;
static jmethodID resultDataGetMessage = nullptr;

// UserInfo 构造函数和方法ID
static jmethodID userInfoConstructor = nullptr;

// 回调接口方法ID
static jmethodID onSuccessMethod = nullptr;
static jmethodID onProgressMethod = nullptr;
static jmethodID onErrorMethod = nullptr;

// 初始化全局引用
extern "C" JNIEXPORT jstring

JNICALL
Java_com_roy_jni_1demo_JniManager_initJNI(JNIEnv *env, jobject /* this */) {

// 获取ResultData类引用
    jclass localResultDataClass = env->FindClass("com/roy/jni_demo/ResultData");
    if (localResultDataClass == nullptr) {
        // 如果找不到类，FindClass会抛出异常，但为了安全，我们返回错误信息
        return env->NewStringUTF("Error: FindClass ResultData failed");
    }
    resultDataClass = (jclass) env->NewGlobalRef(localResultDataClass);
    env->DeleteLocalRef(localResultDataClass); // 及时删除局部引用

// 获取UserInfo类引用
    jclass localUserInfoClass = env->FindClass("com/roy/jni_demo/UserInfo");
    userInfoClass = (jclass) env->NewGlobalRef(localUserInfoClass);
    env->DeleteLocalRef(localUserInfoClass);
// 获取回调接口类引用
    jclass localCallbackClass = env->FindClass("com/roy/jni_demo/CallbackInterface");
    callbackInterfaceClass = (jclass) env->NewGlobalRef(localCallbackClass);
    env->DeleteLocalRef(localCallbackClass);

// 获取ResultData构造函数和方法
    resultDataConstructor = env->GetMethodID(resultDataClass, "<init>", "(ILjava/lang/String;JZ)V");

// 获取UserInfo构造函数
    userInfoConstructor = env->GetMethodID(userInfoClass, "<init>",
                                           "(Ljava/lang/String;Ljava/lang/String;I)V");

// 获取回调接口方法
    onSuccessMethod = env->GetMethodID(callbackInterfaceClass, "onSuccess",
                                       "(Lcom/roy/jni_demo/ResultData;)V");
    if (onSuccessMethod == nullptr) {
        return env->NewStringUTF("Error: GetMethodID onSuccess failed");
    }

    onProgressMethod = env->GetMethodID(callbackInterfaceClass, "onProgress",
                                        "(ILcom/roy/jni_demo/UserInfo;)V");
    if (onProgressMethod == nullptr) {
        return env->NewStringUTF("Error: GetMethodID onProgress failed");
    }

    onErrorMethod = env->GetMethodID(callbackInterfaceClass, "onError",
                                     "(ILjava/lang/String;Lcom/roy/jni_demo/ResultData;)V");

    if (onErrorMethod == nullptr) {
        return env->NewStringUTF("Error: GetMethodID onError failed");
    }

    return env->NewStringUTF("JNI初始化成功");
}

// 清理全局引用
extern "C" JNIEXPORT jstring

JNICALL
Java_com_roy_jni_1demo_JniManager_cleanupJNI(
        JNIEnv
        *env,
        jobject /* this */) {
    if (resultDataClass != nullptr) {
        env->
                DeleteGlobalRef(resultDataClass);
        resultDataClass = nullptr;
    }
    if (userInfoClass != nullptr) {
        env->
                DeleteGlobalRef(userInfoClass);
        userInfoClass = nullptr;
    }
    if (callbackInterfaceClass != nullptr) {
        env->
                DeleteGlobalRef(callbackInterfaceClass);
        callbackInterfaceClass = nullptr;
    }
}

// 主要的JNI方法实现
extern "C" JNIEXPORT void

JNICALL
Java_com_roy_jni_1demo_JniManager_processDataWithCallback(JNIEnv
                                                          *env,
                                                          jobject thiz, jstring
                                                          input,
                                                          jstring user_id,
                                                          jobject
                                                          callback) {

    const char *inputStr = env->GetStringUTFChars(input, nullptr);
    const char *userIdStr = env->GetStringUTFChars(user_id, nullptr);

    LOGI("开始处理数据: %s, 用户ID: %s", inputStr, userIdStr);

    try {
// 创建全局引用，避免回调时对象被回收
        jobject globalCallback = env->NewGlobalRef(callback);

// 模拟进度更新
        for (
                int progress = 0;
                progress <= 100; progress += 20) {


// 创建UserInfo对象
            jstring userName = env->NewStringUTF("测试用户");
            jstring jUserId = env->NewStringUTF(userIdStr);
            jobject userInfo = env->NewObject(userInfoClass, userInfoConstructor,
                                              jUserId, userName, 25);

// 回调进度
            env->
                    CallVoidMethod(globalCallback, onProgressMethod,
                                   progress, userInfo
            );

            env->
                    DeleteLocalRef(userName);
            env->
                    DeleteLocalRef(jUserId);
            env->
                    DeleteLocalRef(userInfo);

            if (env->

                    ExceptionCheck()

                    ) {
                env->

                        ExceptionDescribe();

                env->

                        ExceptionClear();

            }
        }

// 模拟处理结果
        bool success = (std::rand() % 2) == 0;

        if (success) {
// 创建成功的ResultData对象
            jstring successMessage = env->NewStringUTF("数据处理成功完成");
            jobject resultData = env->NewObject(resultDataClass, resultDataConstructor,
                                                1, successMessage,
                                                (jlong) time(nullptr), JNI_TRUE);

// 回调成功
            env->
                    CallVoidMethod(globalCallback, onSuccessMethod, resultData
            );

            env->
                    DeleteLocalRef(successMessage);
            env->
                    DeleteLocalRef(resultData);
        } else {
// 创建错误的ResultData对象
            jstring errorMessage = env->NewStringUTF("处理过程中发生错误");
            jobject errorData = env->NewObject(resultDataClass, resultDataConstructor,
                                               -1, errorMessage,
                                               (jlong) time(nullptr), JNI_FALSE);

            jstring errorMsg = env->NewStringUTF("未知错误");
            env->
                    CallVoidMethod(globalCallback, onErrorMethod,
                                   500, errorMsg, errorData);

            env->
                    DeleteLocalRef(errorMessage);
            env->
                    DeleteLocalRef(errorData);
            env->
                    DeleteLocalRef(errorMsg);
        }

// 清理全局引用
        env->
                DeleteGlobalRef(globalCallback);

    } catch (...) {
        LOGE("JNI回调过程中发生异常");
    }

    env->
            ReleaseStringUTFChars(input, inputStr
    );
    env->
            ReleaseStringUTFChars(user_id, userIdStr
    );
}


extern "C" JNIEXPORT jstring

JNICALL
Java_com_roy_jni_1demo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}









