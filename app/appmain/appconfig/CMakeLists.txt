
INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog
)

INCLUDE_DIRECTORIES(
	../log
	../utils
	../common
)

SET(APPCONFIG_SRC
	TAppConfig.cpp
)


ADD_LIBRARY(appconfig STATIC 				
	${APPCONFIG_SRC}
)
