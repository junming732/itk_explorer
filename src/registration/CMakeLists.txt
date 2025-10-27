# Registration module CMakeLists.txt

# Stage 9: Registration library for multi-modal registration
add_library(registration_lib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/MultiModalRegistration.cpp
)
target_link_libraries(registration_lib PRIVATE ${ITK_LIBRARIES})
target_include_directories(registration_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Registration executables
set(REG_MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/reg_main.cpp)
add_executable(itk_register ${REG_MAIN_SOURCES})
target_link_libraries(itk_register PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_register PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_register PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# B-Spline registration
set(BSPLINE_MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/bspline_main.cpp)
add_executable(itk_bspline_register ${BSPLINE_MAIN_SOURCES})
target_link_libraries(itk_bspline_register PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_bspline_register PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_bspline_register PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Batch registration
set(BATCH_MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/batch_main.cpp)
add_executable(itk_batch_register ${BATCH_MAIN_SOURCES})
target_link_libraries(itk_batch_register PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_batch_register PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_batch_register PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Stage 9: Multi-modal registration
set(MULTIMODAL_MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/multimodal_reg_main.cpp)
add_executable(itk_multimodal_register ${MULTIMODAL_MAIN_SOURCES})
target_link_libraries(itk_multimodal_register PRIVATE
    ${ITK_LIBRARIES}
    registration_lib
    landmarks_lib
    evaluation_lib
)
target_include_directories(itk_multimodal_register PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_multimodal_register PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})