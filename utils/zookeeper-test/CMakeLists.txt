set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ERROR_ON_UNDEFINED_SYMBOLS=0")
add_executable(zk-test main.cpp)
target_link_libraries(zk-test PRIVATE clickhouse_common_zookeeper_no_log)
