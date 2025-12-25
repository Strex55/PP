cmake_minimum_required(VERSION 3.10)
project(BlurLab4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Отключаем предупреждения безопасности
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG:NONE")
endif()

add_executable(Blur main.cpp)

target_compile_features(Blur PRIVATE cxx_std_17)