#!/bin/bash

function make_build()
{
	ECHO ""
	ECHO "=============build glog-0.3.5============="
	
	SRC_FILE=${PROJECT_DIR}/third_party/tarball/glog-0.3.5.tar.gz
	DST_FILE=${OUT_3RD_DIR}/obj/glog-0.3.5
	
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
		./configure --host=${PROJECT_CROSS_COMPILE} --prefix=${OUT_3RD_DIR}/usr  --enable-static
	fi
	cd ${DST_FILE}
	make
	make install	

	build_lib_end
}

make_build