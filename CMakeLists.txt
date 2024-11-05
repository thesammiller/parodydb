cmake_minimum_required(VERSION 3.14)
project(Parody)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


enable_testing()


add_subdirectory(googletest)

add_executable(
        parody
        src/main.cpp
        src/node.cpp
        src/node.h
        src/structs.h
        src/key.cpp
        src/linklist.h
        src/btree.cpp
        src/btree.h
        src/tnode.cpp
        src/parody.cpp
)


include(GoogleTest)
