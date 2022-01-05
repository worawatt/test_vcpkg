cmake_minimum_required(VERSION 3.18)

project(test_vcpkg CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(
  WIN32
  _WIN32_WINNT=0x0600
)

add_executable(main main.cpp)

find_package(Boost REQUIRED system regex)

include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${Boost_LIBRARIES})
