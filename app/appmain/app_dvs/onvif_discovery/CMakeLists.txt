INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/gsoap
)

INCLUDE_DIRECTORIES(

)


LINK_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/lib	
)

ADD_EXECUTABLE(test_main
	discoveryC.cpp
	discoveryClient.cpp
	discoveryServer.cpp
	onvif_server_intf.cpp
)

TARGET_LINK_LIBRARIES(test_main
	-lgsoap++
	-lpthread
	-lm
	-ldl
)


INSTALL(TARGETS test_main RUNTIME DESTINATION bin)