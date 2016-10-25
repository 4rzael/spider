# clent and server

set(SPIDER_SRC ${SPIDER_SRC}
  "${CMAKE_CURRENT_LIST_DIR}/server/src/resolver.cpp"
  )

set(SPIDER_INCLUDE_DIR ${SPIDER_INCLUDE_DIR}
  "${CMAKE_SOURCE_DIR}/boost/server/include"
  )

set(SPIDER_LIB ${SPIDER_LIB}
  "-lboost_system"
  "-lboost_thread"
  "-lboost_regex"
  "-lboost_date_time"
  "-lpthread"
)

set(SPIDER_CXX_FLAGS ${SPIDER_CXX_FLAGS})