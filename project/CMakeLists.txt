cmake_minimum_required(VERSION 2.8)

project(chipDJ)

find_package(Boost 1.36.0 REQUIRED)
find_package(OpenCV REQUIRED)

add_subdirectory(src)
