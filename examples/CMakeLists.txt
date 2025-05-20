# echo_server sample

message(STATUS "Note: This example uses Boost.Asio for cross-platform signal handling")

find_package(Boost REQUIRED COMPONENTS system)

add_executable(echo_server echo_server.cpp)
target_link_libraries(echo_server
  PRIVATE
    ${PROJECT_NAME}
    Boost::system
) 