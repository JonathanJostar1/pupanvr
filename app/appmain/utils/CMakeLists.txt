INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog)

INCLUDE_DIRECTORIES(
	../log
)

SET(UTILS_SRC
	VThreadBase.cpp	
	TMutex.cpp
	TMutexLocker.cpp
)


ADD_LIBRARY(utils STATIC 				
	${UTILS_SRC}
)
