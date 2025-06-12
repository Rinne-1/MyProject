cmake_minimum_required(VERSION 3.15)
project(TcpServer)
set(CMAKE_CXX_STANDARD 11)
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)
include_directories(${PROJECT_SOURCE_DIR}/inc)
set(EXECUTABLE_OUTPUT_PATH D:\\lrq\\visualCode\\MyCode\\TcpServer)

add_executable(app ${SRC})
target_link_libraries(app odbc32 ws2_32)
