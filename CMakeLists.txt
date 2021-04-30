cmake_minimum_required(VERSION 3.10)
project(master_test)

add_subdirectory(./googletest)
include_directories(./googletest/googletest/include)
include_directories(./googletest/googlemock/include)

set(CMAKE_CXX_STANDARD 11)
set(LIBRARIES gtest gtest_main pthread)


add_executable(master_test master_test.cpp)
target_link_libraries(master_test ${LIBRARIES})