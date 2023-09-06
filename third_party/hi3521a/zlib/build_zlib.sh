#!/bin/bash


function make_zlib()
{
	ECHO ""
	ECHO "=============build zlib-1.2.11============="
	
	SRC_FILE=${PROJECT_DIR}/third_party/tarball/zlib-1.2.11.tar.gz
	DST_FILE=${OUT_3RD_DIR}/obj/zlib-1.2.11
	
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
		CC=${PROJECT_CROSS_COMPILE}-gcc CFLAGS=" -fPIC " ./configure --static  --prefix=${OUT_3RD_DIR}/usr
	fi

	cd ${DST_FILE} 
	make
	make install
	
	build_lib_end
}

make_zlib