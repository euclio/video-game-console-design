cmake_minimum_required(VERSION 2.8)

project(handDetection)

find_package(OpenCV REQUIRED)
find_package(CUDA REQUIRED)

include_directories(third_party/rapidjson/include)

add_subdirectory(src)
