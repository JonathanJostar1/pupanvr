INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/freetype2)

INCLUDE_DIRECTORIES(
	../log
	../utils
	lvgl
	${PROJECT_SOURCE_DIR}/lvgl/
	${PROJECT_SOURCE_DIR}/lvgl/lvgl
)

SET(VIEW_SRC
	lvgl/SourceHanSerifSC_Regular_12.c
	lvgl/SourceHanSerifSC_Regular_14.c
	lvgl/SourceHanSerifSC_Regular_16.c
	lvgl/SourceHanSerifSC_Regular_20.c
	lvgl/fontawesome_webfont_12.c
	lvgl/fontawesome_webfont_14.c
	lvgl/fontawesome_webfont_16.c
	lvgl/fontawesome_webfont_20.c
	lvgl/lv_freetype.c
	view_main.cpp
	view_lvgl.cpp
	TViewFontUtils.cpp
	TViewApp.cpp
	TViewObject.cpp
	TViewMainWindow.cpp
	TViewLogin.cpp
	TViewPlayer.cpp
	TViewMultiVideoManage.cpp
)


ADD_LIBRARY(view STATIC 				
	${VIEW_SRC}
)
