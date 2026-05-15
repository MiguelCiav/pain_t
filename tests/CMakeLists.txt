add_executable(run_tests test_logic.cpp)

target_link_libraries(run_tests PRIVATE 
    Catch2::Catch2WithMain
    glfw  
    glm
    glad
    imgui
    OpenGL::GL
)

target_include_directories(run_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/pain_t/src
)
