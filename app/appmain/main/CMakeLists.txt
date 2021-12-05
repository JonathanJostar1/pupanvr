INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog
)

INCLUDE_DIRECTORIES(
	../log
	../utils
	../modules
	../appconfig
	../common
	../record
)


LINK_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/lib	
)

ADD_EXECUTABLE(pupa_main
	main.cpp
)

TARGET_LINK_LIBRARIES(pupa_main
	record
	modules
	appconfig
	common
	utils
	log
	-lglog
	-liniparser
	-lpthread
	-lm
	-ldl
)


INSTALL(TARGETS pupa_main RUNTIME DESTINATION bin)