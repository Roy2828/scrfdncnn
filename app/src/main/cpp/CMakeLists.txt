
cmake_minimum_required(VERSION 3.22.1)

project("jni_demo")


add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        native-lib.cpp)


target_link_libraries(${CMAKE_PROJECT_NAME}

        android
        log)