INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/freetype2)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/libpng16)

INCLUDE_DIRECTORIES(
	../log
	../utils
	../viewLvglRes
	lvgl
	${PROJECT_SOURCE_DIR}/lvgl/
	${PROJECT_SOURCE_DIR}/lvgl/lvgl
)

SET(VIEW_SRC
	view_main.cpp
	view_lvgl.cpp
	TViewScreenSnap.cpp
	THciBLLProcess.cpp
	TViewFontUtils.cpp
	TViewApp.cpp
	TViewObject.cpp
	TViewStyle.cpp
	TViewMainWindow.cpp
	TViewLogin.cpp
	TViewPlayer.cpp
	TViewMultiVideoManage.cpp
	TViewSysSetFrame.cpp
)


ADD_LIBRARY(view STATIC 				
	${VIEW_SRC}
)
