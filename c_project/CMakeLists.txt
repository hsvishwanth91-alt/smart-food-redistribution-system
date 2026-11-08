cmake_minimum_required(VERSION 3.10)
project(SmartFoodRedistributionSystem C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_definitions(-D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE)

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(food_redistribution ${SOURCES})

if(MSVC)
    target_compile_options(food_redistribution PRIVATE /W4)
else()
    target_compile_options(food_redistribution PRIVATE -Wall -Wextra -pedantic)
endif()
