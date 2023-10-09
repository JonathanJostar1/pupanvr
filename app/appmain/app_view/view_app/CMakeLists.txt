INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog
)

INCLUDE_DIRECTORIES(
	../../log
	../../utils
	../../appconfig
	../../common
	../../hal_media/include
	../view
	../viewLvglRes
)


LINK_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/lib
	${PROJECT_SOURCE_DIR}/appmain/bsp/${CHIP}/lib
	${PROJECT_SOURCE_DIR}/lvgl/lib/${CHIP}
)

ADD_EXECUTABLE(dvs_view
	dvs_view.cpp
)

if (${CHIP} STREQUAL "hi3536c")
	set(BOARD_LIBS VoiceEngine upvqe tde mpi md ive hive_RNR hive_RES hive_MBC hive_HPF hive_HDR 
		hive_EQ hive_common hive_ANR hive_AGC hive_AEC hdmi dnvqe aacdec aacenc jpeg jpeg6b) 
elseif((${CHIP} STREQUAL "mc6850"))
	set(BOARD_LIBS mpi nna audio_codec) 
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable")

TARGET_LINK_LIBRARIES(dvs_view
	view
	viewLvglRes
	appconfig
	common
	utils
	#halMedia
	#${BOARD_LIBS}
	log
	-lts
	-llv_demos
	-llv_drivers
	-llvgl
	-lfreetype
	-lpng
	-ljpeg
	-lz
	-lglog
	-lpthread
	-lm
	-ldl
)



INSTALL(TARGETS dvs_view RUNTIME DESTINATION bin)