cmake_minimum_required(VERSION 3.15)

project(laba5_oop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.0
)
FetchContent_MakeAvailable(googletest)

add_executable(${CMAKE_PROJECT_NAME}_exe src/main.cpp)
target_include_directories(${CMAKE_PROJECT_NAME}_exe PRIVATE include)

enable_testing()
target_include_directories(tests PRIVATE include)
add_executable(tests test/test.cpp)
target_link_libraries(tests PRIVATE gtest_main gtest)
add_test(NAME ProjectTests COMMAND tests)
