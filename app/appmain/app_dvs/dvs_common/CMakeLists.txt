
INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog
)

INCLUDE_DIRECTORIES(
	../../log
	../../utils
	../../common
	../../appconfig
	../dvs_common
)

SET(DVS_COMMON_SRC
	TAppComm.h
	TAppComm.cpp
	TDvsAppConfig.h
	TDvsAppConfig.cpp
)


ADD_LIBRARY(dvs_common STATIC 				
	${DVS_COMMON_SRC}
)
