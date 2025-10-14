# src/io/CMakeLists.txt
set(IO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/io_main.cpp
)

add_executable(itk_io ${IO_SOURCES})
target_link_libraries(itk_io PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_io PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_io PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
