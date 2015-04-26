set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CUDA_PROPAGATE_HOST_FLAGS OFF)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(chipDJ
    hand.cpp
    handDetection.cpp
    main.cpp
    median.cpp
    silhouette.cpp)

target_link_libraries(chipDJ ${OpenCV_LIBS})

set_target_properties(chipDJ
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
