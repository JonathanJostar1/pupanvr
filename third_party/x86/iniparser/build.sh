#!/bin/bash

function make_build()
{
	ECHO ""
	ECHO "=============build iniparser-4.1============="
	
	SRC_FILE=${PROJECT_DIR}/third_party/tarball/iniparser-4.1.tar.gz
	DST_FILE=${OUT_3RD_DIR}/obj/iniparser-4.1
	
	ECHO ${SRC_FILE}
	
	if [ ! -e ${SRC_FILE} ]
	then
		ERR "not find ${SRC_FILE}"
		exit -1
	fi
	
	if [ ! -d ${DST_FILE} ]
	then
		tar -xf ${SRC_FILE} -C ${OUT_3RD_DIR}/obj
	
		cd ${DST_FILE}
		#./configure --host=${PROJECT_CROSS_COMPILE} --prefix=${OUT_3RD_DIR}/usr  --enable-static
		
	fi
	cd ${DST_FILE}
	make CC=gcc CXX=g++
	mkdir ${OUT_3RD_DIR}/usr/include/iniparse -p
	cp ${OUT_3RD_DIR}/obj/iniparser-4.1/libiniparser.a  ${OUT_3RD_DIR}/obj/iniparser-4.1/libiniparser.so.1  ${OUT_3RD_DIR}/usr/lib
	cp ${OUT_3RD_DIR}/obj/iniparser-4.1/src/*.h ${OUT_3RD_DIR}/usr/include/iniparse/
	build_lib_end
}

make_build