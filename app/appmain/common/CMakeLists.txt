
INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog
)

INCLUDE_DIRECTORIES(
	../log
	../utils
)

SET(COMMON_SRC
	TAppComm.cpp
)


ADD_LIBRARY(common STATIC 				
	${COMMON_SRC}
)
