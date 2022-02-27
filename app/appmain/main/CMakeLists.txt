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
)


LINK_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/lib	
	${PROJECT_SOURCE_DIR}/appmain/bsp/${CHIP}/lib
	
	${PROJECT_SOURCE_DIR}/lvgl/lib
)

ADD_EXECUTABLE(pupa_main
	main.cpp
)


set(HI3536C_MPP VoiceEngine upvqe tde mpi md ive hive_RNR hive_RES hive_MBC hive_HPF hive_HDR hive_EQ hive_common hive_ANR hive_AGC hive_AEC hdmi dnvqe aacdec aacenc jpeg jpeg6b) 

TARGET_LINK_LIBRARIES(pupa_main
	view
	viewLvglRes
	record
	modules
	halMedia
	appconfig
	common
	utils
	${HI3536C_MPP}
	log
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