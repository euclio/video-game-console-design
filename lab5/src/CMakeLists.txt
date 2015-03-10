set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Don't send the C++11 flag to NVCC, or it will choke.
set(CUDA_PROPAGATE_HOST_FLAGS OFF)

cuda_compile(FILTER_O medianBlur.cu)

add_executable(handDetection
    main.cpp
    handGesture.cpp
    myImage.cpp
    roi.cpp
    socketServer.cpp
    ${FILTER_O}
)

target_link_libraries(handDetection ${OpenCV_LIBS})

set_target_properties(handDetection
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

