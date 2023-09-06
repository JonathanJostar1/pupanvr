#!/bin/bash


function make_live()
{
	ECHO ""
	ECHO "=============build live555============="
	
	SRC_FILE=${PROJECT_DIR}/third_party/tarball/live555-latest.tar.gz
	DST_FILE=${OUT_3RD_DIR}/obj/live
	
	ECHO ${SRC_FILE}
	
	if [ ! -e ${SRC_FILE} ]
	then
		ERR "not find ${SRC_FILE}"
		exit -1
	fi
	
	if [ ! -d ${DST_FILE} ]
	then
		tar -zxf ${SRC_FILE} -C ${OUT_3RD_DIR}/obj
		cd ${DST_FILE}
		#sudo cp ${PROJECT_DIR}/third_party/${DEV_CHIP}/live555/config.armlinux ${OUT_3RD_DIR}/obj/live/
		#make CROSS_COMPILE=${PROJECT_CROSS_COMPILE}- INCLUDES=${OUT_3RD_DIR}/usr/include
	fi

	cd ${DST_FILE}
	rm Makefile
	sudo cp -rd ${PROJECT_DIR}/third_party/${DEV_CHIP}/live555/live/* ${OUT_3RD_DIR}/obj/live/
	./genMakefiles armlinux2
	make CROSS_COMPILE=${PROJECT_CROSS_COMPILE}- OPENSSLINCLUDE=-I${OUT_3RD_DIR}/usr/include
	make install
	
	build_lib_end
}

make_live