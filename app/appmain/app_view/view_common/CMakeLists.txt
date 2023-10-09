INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/freetype2)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/hal_media/include)

INCLUDE_DIRECTORIES(
	../../log
	../../appconfig
	${PROJECT_SOURCE_DIR}/lvgl/
	${PROJECT_SOURCE_DIR}/lvgl/lvgl
)

SET(VIEW_COMMON
	TViewConfig.h
	TViewConfig.cpp
)

ADD_LIBRARY(view_common STATIC 				
	${VIEW_COMMON}
)
