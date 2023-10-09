INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/freetype2)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/libpng16)

INCLUDE_DIRECTORIES(
	../../log
	../../utils
	../viewLvglRes
	bll_view
	lvgl
	${PROJECT_SOURCE_DIR}/lvgl/
	${PROJECT_SOURCE_DIR}/lvgl/lvgl
)

SET(VIEW_SRC
	bll_view/THciBLLProcess.h
	bll_view/THciBLLProcess.cpp
	res/arrow_48.c
	view_main.cpp
	view_lvgl.cpp
	TViewScreenSnap.cpp
	
	TViewFontUtils.cpp
	TViewApp.h
	TViewApp.cpp
	TViewObject.h
	TViewObject.cpp
	TViewStyle.h
	TViewStyle.cpp
	TViewMainWindow.h
	TViewMainWindow.cpp
	TViewLogin.h
	TViewLogin.cpp
	TViewPlayer.h
	TViewPlayer.cpp
	TViewMultiVideoManage.h
	TViewMultiVideoManage.cpp
	TViewSysSetFrame.h
	TViewSysSetFrame.cpp
	TViewPlayToolBar.h
	TViewPlayToolBar.cpp
	TViewConfigMainWindow.h
	TViewConfigMainWindow.cpp
	TViewConfigWinBaseFrame.h
	TViewConfigWinBaseFrame.cpp
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable")

ADD_LIBRARY(view STATIC ${VIEW_SRC})


