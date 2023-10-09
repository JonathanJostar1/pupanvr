#!/bin/bash

function make_jsonrpc_c()
{
	ECHO ""
	ECHO "=============build jsonrpc-c-master.tar============="
	
	SRC_FILE=${PROJECT_DIR}/third_party/tarball/jsonrpc-c-master.tar.gz
	DST_FILE=${OUT_3RD_DIR}/obj/jsonrpc-c-master
	
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
		aclocal
		autoconf
		autoheader
		autoreconf  -ivf
		automake --add-missing
		
		./configure --host=${PROJECT_CROSS_COMPILE} --prefix=${OUT_3RD_DIR}/usr --with-libev=${OUT_3RD_DIR}/usr
	fi
	cd ${DST_FILE}
	make
	make install	

	build_lib_end
}

make_jsonrpc_c