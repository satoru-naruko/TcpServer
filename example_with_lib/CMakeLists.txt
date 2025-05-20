cmake_minimum_required(VERSION 3.20)
project(TcpServerExamples VERSION 1.0.0 LANGUAGES CXX)

# C++17を指定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# インストールされているライブラリを検索
find_package(Boost 1.71.0 REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)

# TcpServerライブラリのパスを探す（必要に応じて設定）
find_library(TCPSERVER_LIBRARY
  NAMES TcpServer
  PATHS /usr/local/lib
)

# TcpServerライブラリのヘッダーパスを設定
include_directories(
  /usr/local/include
  ${Boost_INCLUDE_DIRS}
)

# application_serverのビルド
add_executable(application_server application_server.cpp)
target_link_libraries(application_server
  PRIVATE
    ${TCPSERVER_LIBRARY}
    ${Boost_LIBRARIES}
    Threads::Threads
    spdlog::spdlog
) 