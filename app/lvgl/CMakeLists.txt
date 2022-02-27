PROJECT(lvgl)

set(LIB_INSTALL_DIR ${PROJECT_SOURCE_DIR}/lib)

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/../appmain/lib/${CHIP}/include/freetype2
)


set(INC_INSTALL_DIR)


add_definitions("-std=c99 ")

ADD_SUBDIRECTORY(lvgl)
ADD_SUBDIRECTORY(lv_demos)
ADD_SUBDIRECTORY(lv_drivers)

