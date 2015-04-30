cmake_minimum_required(VERSION 2.8)

project(chipDJ)

find_package(Boost 1.36.0 COMPONENTS program_options REQUIRED)
find_package(OpenCV REQUIRED)
find_package(SFML 2 REQUIRED audio)

add_subdirectory(src)
add_subdirectory(assets)
