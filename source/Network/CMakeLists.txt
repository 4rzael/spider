# SocketC

set(SPIDER_SRC ${SPIDER_SRC}
  "${CMAKE_CURRENT_LIST_DIR}/Broker.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/NetManager.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/sockets/Server.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/sockets/Client.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/protocol/Deserializer.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/protocol/Serializer.cpp"
  )

set(SPIDER_INCLUDE_DIR ${SPIDER_INCLUDE_DIR}
  )

set(SPIDER_LIB ${SPIDER_LIB}
  -lpthread
  )

set(SPIDER_CXX_FLAGS ${SPIDER_CXX_FLAGS}
)