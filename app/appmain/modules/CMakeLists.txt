
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog)

INCLUDE_DIRECTORIES(
	../log
	../utils
	../appconfig
)

SET(MODULES_SRC
	TServiceBase.cpp
	TSysServicesManage.cpp
)


ADD_LIBRARY(modules STATIC 				
	${MODULES_SRC}
)
