INCLUDE_DIRECTORIES(
	./include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/libev
)

LINK_DIRECTORIES(
	
)

SET(jsonrpcc_src
	src/cJSON.c
	src/jsonrpc-c.c
)

ADD_LIBRARY(jsonrpcc STATIC 				
	${jsonrpcc_src}
)

INSTALL(TARGETS jsonrpcc DESTINATION "${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/lib")
INSTALL(FILES include/cJSON.h DESTINATION "${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include")
INSTALL(FILES include/jsonrpc-c.h DESTINATION "${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include")