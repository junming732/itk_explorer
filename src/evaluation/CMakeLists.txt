set(EVALUATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics_main.cpp
)

add_executable(itk_metrics ${EVALUATION_SOURCES})
target_link_libraries(itk_metrics PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_metrics PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_metrics PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
