cmake_minimum_required(VERSION 3.18)

project(test_vcpkg CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(
  WIN32
  _WIN32_WINNT=0x0600
)

#add_executable(main main.cpp "curl_send_mail.cpp")
#find_package(Boost REQUIRED system regex)
#include_directories(${Boost_INCLUDE_DIRS})
#target_link_libraries(main PRIVATE ${Boost_LIBRARIES})

find_package(CURL REQUIRED)

add_executable(send_mail "curl_send_mail.cpp")
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(send_mail PRIVATE ${CURL_LIBRARIES})
target_compile_definitions(send_mail PRIVATE SKIP_PEER_VERIFICATION SKIP_HOSTNAME_VERIFICATION)

add_executable(send_mail_tls "curl_send_mail_tls.cpp")
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(send_mail_tls PRIVATE ${CURL_LIBRARIES})
