set(REGISTRATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/reg_main.cpp
)

add_executable(itk_register ${REGISTRATION_SOURCES})
target_link_libraries(itk_register PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_register PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_register PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Deformable registration (B-spline)
add_executable(itk_bspline_register ${CMAKE_CURRENT_SOURCE_DIR}/bspline_main.cpp)
target_link_libraries(itk_bspline_register PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_bspline_register PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_bspline_register PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Batch automation
add_executable(itk_batch_register ${CMAKE_CURRENT_SOURCE_DIR}/batch_main.cpp)
target_link_libraries(itk_batch_register PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_batch_register PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_batch_register PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
