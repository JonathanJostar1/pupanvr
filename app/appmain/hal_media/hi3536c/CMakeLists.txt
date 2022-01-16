
INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog
)

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/bsp/${CHIP}/include
	${PROJECT_SOURCE_DIR}/appmain/hal_media/include
	./common
	./audio/adp
)

SET(HAL_MEDIA_SRC
	common/sample_comm_audio.c	
	common/sample_comm_ivs.c
	common/sample_comm_sys.c
	common/sample_comm_vda.c
	common/sample_comm_vdec.c
	common/sample_comm_venc.c
	common/sample_comm_vo.c
	common/sample_comm_vpss.c
	audio/adp/audio_aac_adp.c
	hal_media.c	
	mpp_sys.c
	mpp_vo.c
	mpp_fb.c
)


ADD_LIBRARY(halMedia STATIC 				
	${HAL_MEDIA_SRC}
)
