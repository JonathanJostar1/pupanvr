INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/live555)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/live555/liveMedia)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/live555/UsageEnvironment)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/live555/BasicUsageEnvironment)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/live555/groupsock)

INCLUDE_DIRECTORIES(
	../../log
	../../utils
	../../modules
	../../appconfig
	../../common
	../dvs_common
	../lib/hi3536c/include/live555
	../lib/hi3536c/include/live555/UsageEnviroment
	../lib/hi3536c/include/live555/liveMedia
	../lib/hi3536c/include/live555/BasicUsageEnvironment
	../lib/hi3536c/include/live555/groupsock

	/home/jhting/work_data/code/PUPANVR/app/appmain/lib/hi3536c/include/live555/groupsock
	/home/jhting/work_data/code/PUPANVR/app/appmain/lib/hi3536c/include/live555/BasicUsageEnvironment
	/home/jhting/work_data/code/PUPANVR/app/appmain/lib/hi3536c/include/live555/liveMedia
	/home/jhting/work_data/code/PUPANVR/app/appmain/lib/hi3536c/include/live555/UsageEnviroment
)

SET(RECORD_SRC
	TPlayView.h
	TPlayView.cpp
	TVideoChannel.h
	TVideoChannel.cpp
	TVideoSource.h
	TVideoSource.cpp
	TRecordManage.h
	TRecordManage.cpp
	TVideoSourceRtsp.h
	TVideoSourceRtsp.cpp
)

add_definitions(-std=c++11 )

ADD_LIBRARY(record STATIC 				
	${RECORD_SRC}
)
