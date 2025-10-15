set(VISUALIZATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/vis_main.cpp
)

add_executable(itk_visualize ${VISUALIZATION_SOURCES})
target_link_libraries(itk_visualize PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_visualize PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_visualize PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
