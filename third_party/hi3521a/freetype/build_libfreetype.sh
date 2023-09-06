#!/bin/bash

function make_libfreetype()
{
	ECHO ""
	ECHO "=============build freetype-2.6.1.tar.gz============="
	
	SRC_FILE=${PROJECT_DIR}/third_party/tarball/freetype-2.6.1.tar.gz
	DST_FILE=${OUT_3RD_DIR}/obj/freetype-2.6.1
	
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
		CFLAGS=-I${OUT_3RD_DIR}/usr/include LDFLAGS=-L${OUT_3RD_DIR}/usr/lib ./configure --host=${PROJECT_CROSS_COMPILE} --prefix=${OUT_3RD_DIR}/usr
	fi
	cd ${DST_FILE}
	make
	make install	

	build_lib_end
}

make_libfreetype