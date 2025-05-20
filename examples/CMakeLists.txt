# echo_server sample
add_executable(echo_server echo_server.cpp)
target_link_libraries(echo_server
  PRIVATE
    ${PROJECT_NAME}
) 