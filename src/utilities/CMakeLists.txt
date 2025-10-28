# Utilities module CMakeLists.txt
# Contains utility tools for testing and preprocessing

# Motion simulation tool for testing motion correction
add_executable(simulate_motion
    ${CMAKE_CURRENT_SOURCE_DIR}/simulate_motion.cpp
)
target_link_libraries(simulate_motion PRIVATE ${ITK_LIBRARIES})
target_include_directories(simulate_motion PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(simulate_motion
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Resample to reference tool for proper multi-modal visualization
add_executable(resample_to_reference
    ${CMAKE_CURRENT_SOURCE_DIR}/resample_to_reference.cpp
)
target_link_libraries(resample_to_reference PRIVATE ${ITK_LIBRARIES})
target_include_directories(resample_to_reference PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(resample_to_reference
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

message(STATUS "Utilities module configured:")
message(STATUS "  - simulate_motion: Motion simulation for testing")
message(STATUS "  - resample_to_reference: Coordinate system transformation")