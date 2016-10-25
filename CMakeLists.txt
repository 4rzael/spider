# Global cmake for spider
cmake_minimum_required (VERSION 2.8)

project (cpp_spider)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
if (NOT ${CMAKE_SOURCE_DIR}/lib)
  file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
endif()

set(SPIDER_SRC "")