cmake_minimum_required(VERSION 3.14)
project(Parody)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



#include(FetchContent)
#FetchContent_Declare(
#        googletest
#        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
#)
#FetchContent_MakeAvailable(googletest)



enable_testing()

#add_library(structs_library structs.h)

add_subdirectory(googletest)

add_executable(
        parody
        src/main.cpp
        src/node.cpp
        src/node.h
        src/structs.h
)
#target_include_directories(test_structs PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
#target_link_libraries(
#        test_structs
#        GTest::gtest_main
#)


include(GoogleTest)
