cmake_minimum_required(VERSION 3.0.0)
project(LearnOpenGL VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "-lGL -lGLEW -lglfw")

add_executable(${PROJECT_NAME} src/main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} glm)

include(CPack)
