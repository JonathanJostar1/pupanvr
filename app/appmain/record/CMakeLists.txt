INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog)

INCLUDE_DIRECTORIES(
	../log
	../utils
	../modules
	../appconfig
	../common
)

SET(RECORD_SRC
	TPlayView.cpp
	TVideoChannel.cpp
	TVideoSource.cpp
	TRecordManage.cpp	
)


ADD_LIBRARY(record STATIC 				
	${RECORD_SRC}
)
