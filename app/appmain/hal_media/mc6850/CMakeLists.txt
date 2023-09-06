
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
	common/gosd.h
	common/list.h
	common/loadbmp.c
	common/loadbmp.h
	common/osd.h
	common/param_config.c
	common/param_config.h
	common/sample_comm_audio.c
	common/sample_comm.h
	common/sample_comm_jpege.c
	common/sample_comm_param.c
	common/sample_comm_param.h
	common/sample_comm_sys.c
	common/sample_comm_vda.c
	common/sample_comm_vdec.c
	common/sample_comm_vdec.h
	common/sample_comm_venc.c
	common/sample_comm_venc_es2ps.c
	common/sample_comm_venc.h
	common/sample_comm_vi.c
	common/sample_comm_vo.c
	common/sample_comm_vo.h
	common/sample_comm_vpss.c
	common/type_def.h
	hal_media.c	
	fy_mpi_sys.h
	fy_mpi_sys.c
	fy_mpi_vo.h
	fy_mpi_vo.c
	fy_mpi_fb.h
	fy_mpi_fb.c
)


ADD_LIBRARY(halMedia STATIC 				
	${HAL_MEDIA_SRC}
)
