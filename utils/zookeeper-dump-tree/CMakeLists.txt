set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ERROR_ON_UNDEFINED_SYMBOLS=0")
add_executable (zookeeper-dump-tree main.cpp ${SRCS})
target_link_libraries(zookeeper-dump-tree PRIVATE clickhouse_common_zookeeper_no_log clickhouse_common_io boost::program_options)
