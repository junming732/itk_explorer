# Evaluation module CMakeLists.txt

# Stage 9: Evaluation library
add_library(evaluation_lib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/LandmarkEvaluation.cpp
)
target_link_libraries(evaluation_lib PRIVATE
    ${ITK_LIBRARIES}
    landmarks_lib
)
target_include_directories(evaluation_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Metrics executable
set(METRICS_MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/metrics_main.cpp)
add_executable(itk_metrics ${METRICS_MAIN_SOURCES})
target_link_libraries(itk_metrics PRIVATE
    ${ITK_LIBRARIES}
    evaluation_lib
)
target_include_directories(itk_metrics PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_metrics PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})