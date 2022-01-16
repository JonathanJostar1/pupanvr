INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog)

INCLUDE_DIRECTORIES(
	../log
	
	/home/jhting/disk2/code/third_party/lvgl/
	/home/jhting/disk2/code/third_party/lvgl/lvgl
)

SET(VIEW_SRC
	view_main.cpp
	view_lvgl.cpp
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
