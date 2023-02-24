set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ERROR_ON_UNDEFINED_SYMBOLS=0")

add_executable (mysqlxx_pool_test mysqlxx_pool_test.cpp)
target_link_libraries (mysqlxx_pool_test PRIVATE mysqlxx)
