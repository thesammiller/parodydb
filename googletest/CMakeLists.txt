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

add_executable(all_tests
                test_structs.cpp
                test_node.cpp
                test_key.cpp
                ../src/node.cpp
                ../src/key.cpp
)
#add_executable(test_node test_node.cpp ../src/node.cpp)
#add_executable(test_key test_key.cpp ../src/key.h ../src/key.cpp ../src/node.cpp)

#target_link_libraries(test_structs gtest gtest_main)
#target_link_libraries(test_node gtest gtest_main)
#target_link_libraries(test_key gtest gtest_main)
target_link_libraries(all_tests gtest gtest_main)

#gtest_discover_tests(test_structs)