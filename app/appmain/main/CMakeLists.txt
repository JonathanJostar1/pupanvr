

INCLUDE_DIRECTORIES()

LINK_DIRECTORIES()

ADD_EXECUTABLE(pupa_main
	main.cpp
)

TARGET_LINK_LIBRARIES(pupa_main
	-lpthread
	-lm
	-ldl
)


INSTALL(TARGETS pupa_main RUNTIME DESTINATION bin)