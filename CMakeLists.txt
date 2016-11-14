# Global cmake for spider
cmake_minimum_required (VERSION 2.8)

project (cpp_spider)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set (SPIDER_SOURCE "${CMAKE_SOURCE_DIR}/source")

set(SPIDER_SRC "")
set(SPIDER_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SPIDER_LIB "")
set(SPIDER_CXX_FLAGS "-g -std=c++11")

include("${SPIDER_SOURCE}/spider/CMakeLists.txt")
include("${SPIDER_SOURCE}/socket/CMakeLists.txt")
include("${SPIDER_SOURCE}/bdd_connect/CMakeLists.txt")
include("${SPIDER_SOURCE}/utils/CMakeLists.txt")
include("${SPIDER_SOURCE}/Network/CMakeLists.txt")
include("${SPIDER_SOURCE}/keyRegister/CMakeLists.txt")

include_directories(${SPIDER_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS ${SPIDER_CXX_FLAGS})

add_executable(cpp_spider ${SPIDER_SRC})

target_link_libraries(cpp_spider
  ${SPIDER_LIB}
  )
