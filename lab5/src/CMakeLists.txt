set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(handDetection
    main.cpp
    handGesture.hpp
    handGesture.cpp
    myImage.hpp
    myImage.cpp
    roi.hpp
    roi.cpp
)

target_link_libraries(handDetection ${OpenCV_LIBS})

set_target_properties(handDetection
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

