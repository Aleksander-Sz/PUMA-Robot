cmake_minimum_required(VERSION 3.20)
project(PumaRobot LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

list(APPEND CMAKE_PREFIX_PATH
    /opt/homebrew/opt/assimp
    /usr/local/opt/assimp
)
find_package(assimp REQUIRED)

find_package(OpenGL REQUIRED)

set(SOURCES
    PumaRobotProject/src/Application.cpp
    PumaRobotProject/StbImage.cpp
    PumaRobotProject/Paths.cpp
    PumaRobotProject/Camera.cpp
    PumaRobotProject/Light.cpp
    PumaRobotProject/Model.cpp
    PumaRobotProject/Particles.cpp
    PumaRobotProject/Robot.cpp
    PumaRobotProject/Shader.cpp
    # GLAD – compiled directly from source (no separate library needed)
    Dependencies/GLAD/src/gl.c
)

add_executable(PumaRobot ${SOURCES})

target_include_directories(PumaRobot PRIVATE
    ${CMAKE_SOURCE_DIR}/Dependencies/GLAD/include
    ${CMAKE_SOURCE_DIR}/Dependencies/glm
    ${stb_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/PumaRobotProject
)

target_link_libraries(PumaRobot PRIVATE
    OpenGL::GL
    glfw
    assimp::assimp
)

if(APPLE)
  target_compile_definitions(PumaRobot PRIVATE GL_SILENCE_DEPRECATION)
  target_link_libraries(PumaRobot PRIVATE
      "-framework Cocoa"
      "-framework IOKit"
      "-framework CoreVideo"
  )

endif()

if(MSVC)
  target_compile_options(PumaRobot PRIVATE /W3)
endif()

add_custom_command(TARGET PumaRobot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/PumaRobotProject/Shaders
        $<TARGET_FILE_DIR:PumaRobot>/Shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/PumaRobotProject/Textures
        $<TARGET_FILE_DIR:PumaRobot>/Textures
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/PumaRobotProject/Models
        $<TARGET_FILE_DIR:PumaRobot>/Models
    COMMENT "Copying shaders, textures and models to build directory"
)
