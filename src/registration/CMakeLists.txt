set(REGISTRATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/reg_main.cpp
)

add_executable(itk_register ${REGISTRATION_SOURCES})
target_link_libraries(itk_register PRIVATE ${ITK_LIBRARIES})
target_include_directories(itk_register PRIVATE ${PROJECT_SOURCE_DIR}/include)
set_target_properties(itk_register PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
