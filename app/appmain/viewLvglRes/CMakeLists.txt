INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/freetype2)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/hal_media/include)

INCLUDE_DIRECTORIES(
	../log
	${PROJECT_SOURCE_DIR}/lvgl/
	${PROJECT_SOURCE_DIR}/lvgl/lvgl
)

SET(VIEW_LVGLRES
	#font/SourceHanSerifSC_Regular_12.c
	#font/SourceHanSerifSC_Regular_14.c
	#font/SourceHanSerifSC_Regular_16.c
	#font/SourceHanSerifSC_Regular_20.c
	#font/fontawesome_webfont_12.c
	#font/fontawesome_webfont_14.c
	#font/fontawesome_webfont_16.c
	#font/fontawesome_webfont_20.c
	view_lvgl_res.c
	evdev_linux.h
	evdev_linux.c
	evdev_tslib.h
	evdev_tslib.c
	halmedia_fbdev.h
	halmedia_fbdev.c
)

add_definitions( -std=c99 )

ADD_LIBRARY(viewLvglRes STATIC 				
	${VIEW_LVGLRES}
)
