add_executable(handDetection
    handGesture.hpp
    handGesture.cpp
    main.hpp
    main.cpp
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

