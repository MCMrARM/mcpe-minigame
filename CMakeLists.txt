cmake_minimum_required(VERSION 3.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wno-unused-function")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_STANDARD 14)

include_directories(minecraft-headers/)
add_library(minigame SHARED src/main.cpp src/MinigameDimension.h src/MinigameDimension.cpp)

target_link_libraries(minigame dl ${CMAKE_SOURCE_DIR}/dep-libs/libminecraftpe.so)