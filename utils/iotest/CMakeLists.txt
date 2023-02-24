set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ERROR_ON_UNDEFINED_SYMBOLS=0")

add_executable (iotest iotest.cpp ${SRCS})
target_link_libraries (iotest PRIVATE clickhouse_common_io)

add_executable (iotest_nonblock iotest_nonblock.cpp ${SRCS})
target_link_libraries (iotest_nonblock PRIVATE clickhouse_common_io)

add_executable (iotest_aio iotest_aio.cpp ${SRCS})
target_link_libraries (iotest_aio PRIVATE clickhouse_common_io)
