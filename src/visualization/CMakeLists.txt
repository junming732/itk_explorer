# Visualization module CMakeLists.txt

# Original visualization executable (only vis_main.cpp)
set(VISUALIZATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/vis_main.cpp
)

add_executable(itk_visualize ${VISUALIZATION_SOURCES})
target_link_libraries(itk_visualize PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_visualize PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_visualize PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Stage 9: Slice extraction executable (separate executable)
set(EXTRACT_SLICE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/extract_slice_main.cpp
)

add_executable(itk_extract_slice ${EXTRACT_SLICE_SOURCES})
target_link_libraries(itk_extract_slice PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_extract_slice PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_extract_slice PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Motion simulation tool for testing motion correction
set(SIMULATE_MOTION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/simulate_motion.cpp
)

add_executable(simulate_motion ${SIMULATE_MOTION_SOURCES})
target_link_libraries(simulate_motion PRIVATE ${ITK_LIBRARIES})
target_include_directories(simulate_motion PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(simulate_motion PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})