# CMakeList.txt : CMake project for proyecto#1, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (pain_t "src/pain_t.cpp" "src/pain_t.h" "src/engine2D.h" "src/engine2D.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET pain_t PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(pain_t PRIVATE include)

target_link_libraries(pain_t PRIVATE 
    glfw  
    glm
    glad
    imgui
    OpenGL::GL
)