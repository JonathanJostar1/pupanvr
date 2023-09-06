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
	../hal_media/include
	../view
	../viewLvglRes
)


LINK_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/lib
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/lib/live555	
	${PROJECT_SOURCE_DIR}/appmain/bsp/${CHIP}/lib
	${PROJECT_SOURCE_DIR}/lvgl/lib/${CHIP}
)

ADD_EXECUTABLE(pupa_main
	main.cpp
)


if (${CHIP} STREQUAL "hi3536c")
	set(BOARD_LIBS VoiceEngine upvqe tde mpi md ive hive_RNR hive_RES hive_MBC hive_HPF hive_HDR 
		hive_EQ hive_common hive_ANR hive_AGC hive_AEC hdmi dnvqe aacdec aacenc jpeg jpeg6b) 
elseif((${CHIP} STREQUAL "mc6850"))
	set(BOARD_LIBS mpi nna audio_codec) 
endif()

set(LIVE555_LIBS liveMedia groupsock BasicUsageEnvironment UsageEnvironment)

TARGET_LINK_LIBRARIES(pupa_main
	view
	viewLvglRes
	record
	account
	modules
	appconfig
	common
	utils
	halMedia
	${BOARD_LIBS}
	log
	${LIVE555_LIBS}
	-llv_demos
	-llv_drivers
	-llvgl
	-lfreetype
	-lpng
	-ljpeg
	-lz
	-lglog
	-liniparser
	-lpthread
	-lm
	-ldl
)



INSTALL(TARGETS pupa_main RUNTIME DESTINATION bin)