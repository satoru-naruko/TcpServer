cmake_minimum_required(VERSION 3.20)
project(TcpServer VERSION 0.1.0 LANGUAGES CXX)

# C++17を指定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ビルドタイプが指定されていない場合はDebugにする
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# コンパイルオプション
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Boost, OpenSSL, スレッドライブラリを検索
find_package(Boost 1.71.0 REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)

# ライブラリのインクルードディレクトリを設定
include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${Boost_INCLUDE_DIRS}
)

# ソースファイルのリスト
set(SOURCES
  src/tcp_server.cpp
  src/internal/connection.cpp
)

# 共有ライブラリをビルド
add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME}
  PUBLIC
    ${Boost_LIBRARIES}
    Threads::Threads
    spdlog::spdlog
)

# バージョン情報を設定
set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

# インストール設定
install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)

# サブディレクトリを追加
add_subdirectory(tests)
add_subdirectory(examples)

# テストを有効化
enable_testing() 