set(FILTER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/filters_main.cpp
)

add_executable(itk_filters ${FILTER_SOURCES})
target_link_libraries(itk_filters PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_filters PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_filters PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(itk_normalize ${CMAKE_CURRENT_SOURCE_DIR}/normalize_main.cpp)
target_link_libraries(itk_normalize PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_normalize PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_normalize PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
