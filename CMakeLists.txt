cmake_minimum_required(VERSION 3.20)
project(TcpServer VERSION 0.1.0 LANGUAGES CXX)

# テストを有効化（プロジェクト設定の早い段階で）
enable_testing()

# C++17を指定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ソースコードをUTF-8として扱う（VS対応）
if(MSVC)
  add_compile_options(/utf-8)
endif()

# Windows APIバージョンを定義（Windows 7以上）
if(WIN32)
  add_definitions(-D_WIN32_WINNT=0x0601)
  # Windowsでのインポート/エクスポート設定
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

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

# vcpkgの検出を改善
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
endif()

# Boost, スレッドライブラリを検索
find_package(Boost 1.71.0 REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)

# ライブラリのインクルードディレクトリを設定
include_directories(
  ${PROJECT_SOURCE_DIR}
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

# テストの再検出を強制
if(BUILD_TESTING)
  include(CTest)
endif() 