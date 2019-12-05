# MakeLists.txt
cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 11)
project(demo LANGUAGES CXX)

find_package(AWSSDK COMPONENTS transcribestreaming)

add_executable(${PROJECT_NAME} "main.cpp" "audio-capture.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE ${AWSSDK_LINK_LIBRARIES})

if(MSVC)
    target_include_directories(${PROJECT_NAME} PRIVATE "portaudio")
    target_link_directories(${PROJECT_NAME} PRIVATE "portaudio")
    target_link_libraries(${PROJECT_NAME} PRIVATE “portaudio_static_x64”) # might have _x86 suffix instead
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE portaudio)
endif()
