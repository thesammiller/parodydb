# 'Google_test' is the subproject name
project(googletest)

set(gtest_SOURCE_DIR, "googletest")
set(CMAKE_CURRENT_SOURCE_DIR, "../src")

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 'Google_Tests_run' is the target name
# 'test1.cpp test2.cpp' are source files with tests

add_executable(test_structs test_structs.cpp ../src/structs.h)



target_link_libraries(test_structs gtest gtest_main)

#gtest_discover_tests(test_structs)