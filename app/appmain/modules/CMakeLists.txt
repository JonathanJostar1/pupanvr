
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/live555)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/live555/liveMedia)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/live555/UsageEnvironment)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/live555/BasicUsageEnvironment)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/live555/groupsock)

INCLUDE_DIRECTORIES(
	../log
	../utils
	../appconfig
	
)

SET(MODULES_SRC
	TServiceBase.cpp
	TSysServicesManage.cpp
	TLive555EnvThread.h
	TLive555EnvThread.cpp
)

add_definitions(-std=c++11 )

ADD_LIBRARY(modules STATIC 				
	${MODULES_SRC}
)
