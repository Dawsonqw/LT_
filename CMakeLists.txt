cmake_minimum_required(VERSION 3.2)
project(LT)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++11 -pg -Wall -g -march=native -O0")
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")

include_directories(./include)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#
#include_directories(Boost_INC /usr/include/boost)
#set(Boost_USE_STATIC_LIBS ON)
#set(BOOST_COMPONENTS date_time chrono filesystem iostreams
#        program_options regex system thread unit_test_framework)
#find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

find_package(Boost REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(./include)
set(SOURCE_LIST
        src/log.cc
        src/config.cc
        src/util.cc
        src/env.cc
        src/mutex.cc
        src/thread.cc
        src/fiber.cc
        src/scheduler.cc
        src/iomanager.cc
        src/timer.cc
        src/hook.cc
        src/fd_manager.cc
        src/bytearray.cc
        src/address.cc
        src/fd_manager.cc
        src/socket.cc
        src/tcp_server.cc
        src/stream.cc
        src/socket_stream.cc
        src/Http.cc
        src/http_parser.cc
        src/Http_session.cc
        src/servlet.cc
        src/uri.cc
        src/Http_connection.cc
        src/Http_server.cc
        src/daemon.cc
        include/http/http-parser/http_parser.c
        )

add_library(LT STATIC ${SOURCE_LIST})
#add_library(LT SHARED ${SOURCE_LIST})

set(LIBRS
        LT
        dl
        yaml-cpp
        pthread
        )

include_directories(./inlcude)
#link_libraries(../lib)  这是一个添加库路径的命令 注意是全路径

add_executable(test_logs tests/test_logs.cpp)
add_dependencies(test_logs LT)
target_link_libraries(test_logs ${LIBRS})


add_executable(test_log tests/test_log.cpp)
add_dependencies(test_log LT)
target_link_libraries(test_log ${LIBRS})

add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread LT)
target_link_libraries(test_thread ${LIBRS})

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config LT)
target_link_libraries(test_config ${LIBRS})

add_executable(test_env tests/test_env.cc)
add_dependencies(test_env LT)
target_link_libraries(test_env ${LIBRS})


add_executable(test_fiber tests/test_fiber.cc)
add_dependencies(test_fiber LT)
target_link_libraries(test_fiber ${LIBRS})

add_executable(test_fiber2 tests/test_fiber2.cc)
add_dependencies(test_fiber2 LT)
target_link_libraries(test_fiber2 ${LIBRS})

add_executable(test_iomanager tests/test_iomanager.cc)
add_dependencies(test_iomanager LT)
target_link_libraries(test_iomanager ${LIBRS})

add_executable(test_iomanager_mul tests/test_iomanager_mul.cpp)
add_dependencies(test_iomanager_mul LT)
target_link_libraries(test_iomanager_mul ${LIBRS})


add_executable(test_scheduler tests/test_scheduler.cc)
add_dependencies(test_scheduler LT)
target_link_libraries(test_scheduler ${LIBRS})

add_executable(test_timer tests/test_timer.cc)
add_dependencies(test_timer LT)
target_link_libraries(test_timer ${LIBRS})

add_executable(test_hook tests/test_hook.cc)
add_dependencies(test_hook LT)
target_link_libraries(test_hook ${LIBRS})


add_executable(test_util tests/test_util.cpp)
add_dependencies(test_util LT)
target_link_libraries(test_util ${LIBRS})

add_executable(test_address tests/test_address.cc)
add_dependencies(test_address LT)
target_link_libraries(test_address ${LIBRS})


add_executable(test_bytearray tests/test_bytearray.cc)
add_dependencies(test_bytearray LT)
target_link_libraries(test_bytearray ${LIBRS})

add_executable(test_socket_tcp_c tests/test_socket_tcp_client.cc)
add_dependencies(test_socket_tcp_c LT)
target_link_libraries(test_socket_tcp_c ${LIBRS})

add_executable(test_socket_tcp_s tests/test_socket_tcp_server.cc)
add_dependencies(test_socket_tcp_s LT)
target_link_libraries(test_socket_tcp_s ${LIBRS})


add_executable(test_tcp_server tests/test_tcp_server.cc)
add_dependencies(test_tcp_server LT)
target_link_libraries(test_tcp_server ${LIBRS})


add_executable(test_http tests/test_http.cc)
add_dependencies(test_http LT)
target_link_libraries(test_http ${LIBRS})


add_executable(test_http_parser tests/test_http_parser.cc)
add_dependencies(test_http_parser LT)
target_link_libraries(test_http_parser ${LIBRS})

add_executable(test_http_server tests/test_http_server.cc)
add_dependencies(test_http_server LT)
target_link_libraries(test_http_server ${LIBRS})


add_executable(test_http_connection tests/test_http_connection.cc)
add_dependencies(test_http_connection LT)
target_link_libraries(test_http_connection ${LIBRS})


add_executable(test_uri tests/test_uri.cc)
add_dependencies(test_uri LT)
target_link_libraries(test_uri ${LIBRS})

add_executable(test_daemon tests/test_daemon.cc)
add_dependencies(test_daemon LT)
target_link_libraries(test_daemon ${LIBRS})

add_executable(MAIN TEST.cpp)