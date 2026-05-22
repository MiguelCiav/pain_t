file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/pain_t/src/*.cpp"
)

list(REMOVE_ITEM PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/pain_t/src/main.cpp")
list(REMOVE_ITEM PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/pain_t/src/scene/pain_t.cpp") 
list(REMOVE_ITEM PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/pain_t/src/scene/app.cpp") 

add_executable(run_tests test_figures.cpp ${PROJECT_SOURCES})

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
