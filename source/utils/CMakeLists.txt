# clent and server

set(SPIDER_SRC ${SPIDER_SRC}
  "${CMAKE_CURRENT_LIST_DIR}/stringManager.cpp"
  )

set(SPIDER_INCLUDE_DIR ${SPIDER_INCLUDE_DIR}
  )

set(SPIDER_LIB ${SPIDER_LIB}
  -lpqxx
  -lpq
  )

set(SPIDER_CXX_FLAGS ${SPIDER_CXX_FLAGS}
  )
