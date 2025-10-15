set(SEGMENTATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/seg_main.cpp
)

add_executable(itk_segment ${SEGMENTATION_SOURCES})
target_link_libraries(itk_segment PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_segment PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_segment PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
