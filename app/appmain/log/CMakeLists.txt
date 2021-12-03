INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog)

SET(LOG_SRC
	tlog.cpp
)


ADD_LIBRARY(log STATIC 				
	${LOG_SRC}
)
