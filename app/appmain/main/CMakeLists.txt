INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog
)
INCLUDE_DIRECTORIES(
	../log
)

LINK_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/lib	
)

ADD_EXECUTABLE(pupa_main
	main.cpp
)

TARGET_LINK_LIBRARIES(pupa_main
	log
	-lglog
	-lpthread
	-lm
	-ldl
)


INSTALL(TARGETS pupa_main RUNTIME DESTINATION bin)