# clent and server

set(SPIDER_SRC ${SPIDER_SRC}
  "${CMAKE_CURRENT_LIST_DIR}/serverTcpSocket.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/clientTcpSocket.cpp"
  )

set(SPIDER_INCLUDE_DIR ${SPIDER_INCLUDE_DIR}
  )

set(SPIDER_LIB ${SPIDER_LIB}
  -lboost_system
  -lboost_thread
  -lpthread
  )

set(SPIDER_CXX_FLAGS ${SPIDER_CXX_FLAGS}
  )