INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/gsoap
)

set(CMAKE_C_COMPILER gcc)  
set(CMAKE_CXX_COMPILER g++)  

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/onvif_client/plugin
	${PROJECT_SOURCE_DIR}/appmain/onvif_client
)


LINK_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/lib	
)

ADD_EXECUTABLE(test_client_main
	discoveryC.c
	discoveryClient.c
	plugin/wsaapi.c
	plugin/wsddapi.c
	onvif_client_interface.c
)

TARGET_LINK_LIBRARIES(test_client_main
	-lgsoap
	-lpthread
	-lm
	-ldl
)


INSTALL(TARGETS test_client_main RUNTIME DESTINATION bin)