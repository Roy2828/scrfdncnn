project(scrfdncnn)

cmake_minimum_required(VERSION 3.12)

# 启用C++异常
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/opencv-mobile-2.4.13.7-android/sdk/native/jni)
find_package(OpenCV REQUIRED)

set(ncnn_DIR ${CMAKE_SOURCE_DIR}/ncnn-20250916-android-vulkan/${ANDROID_ABI}/lib/cmake/ncnn)
find_package(ncnn REQUIRED)



# 设置ARCface源文件路径
set(ARC_FACE_SRC
        ../jni/arcface/face_api.cpp
        ../jni/arcface/features.cpp
        ../jni/arcface/scrfd.cpp
        ../jni/arcface/utils.cpp
)



add_library(scrfdncnn SHARED scrfdncnn.cpp ${ARC_FACE_SRC} )




target_link_libraries(scrfdncnn ncnn ${OpenCV_LIBS} )
