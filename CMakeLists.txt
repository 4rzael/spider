# Global cmake for spider
cmake_minimum_required (VERSION 2.8)

project (cpp_spider)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(SPIDER_SRC "")
set(SPIDER_INCLUDE_DIR "")
set(SPIDER_LIB "")
set(SPIDER_CXX_FLAGS "")

include(${CMAKE_SOURCE_DIR}/spider/CMakeLists.txt)
include(${CMAKE_SOURCE_DIR}/boost/CMakeLists.txt)

include_directories(${SPIDER_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS ${SPIDER_CXX_FLAGS})

add_executable(cpp_spider ${SPIDER_SRC})

target_link_libraries(cpp_spider
  ${SPIDER_LIB}
  )