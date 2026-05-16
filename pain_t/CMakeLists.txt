file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.h"
)

add_executable(pain_t ${SOURCE_FILES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET pain_t PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(pain_t PRIVATE include src)

target_link_libraries(pain_t PRIVATE 
    glfw  
    glm
    glad
    imgui
    OpenGL::GL
)