set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ERROR_ON_UNDEFINED_SYMBOLS=0")
add_executable(keeper-bench Generator.cpp Runner.cpp Stats.cpp main.cpp)
target_link_libraries(keeper-bench PRIVATE clickhouse_common_zookeeper_no_log)
