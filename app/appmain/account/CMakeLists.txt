
INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/appmain/lib/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include
	${PROJECT_SOURCE_DIR}/appmain/lib/${CHIP}/include/glog
)

INCLUDE_DIRECTORIES(
	../log
	../utils
	../common
	../appconfig
)

SET(ACCOUNT_SRC
	TAccount.h
	TAccount.cpp
)


ADD_LIBRARY(account STATIC 				
	${ACCOUNT_SRC}
)
