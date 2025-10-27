# Landmarks module CMakeLists.txt
# Stage 9: Library-only module for landmark I/O

add_library(landmarks_lib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/LandmarkIO.cpp
)
target_link_libraries(landmarks_lib PRIVATE ${ITK_LIBRARIES})
target_include_directories(landmarks_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# No executable for this module - it's a library only