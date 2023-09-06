#!/bin/sh

function make_openssl()
{
	ECHO ""
	ECHO "=============build openssl-1.1.1u============="
	
	SRC_FILE=${PROJECT_DIR}/third_party/tarball/openssl-1.1.1u.tar.gz
	DST_FILE=${OUT_3RD_DIR}/obj/openssl-1.1.1u
	
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
		CC=${PROJECT_CROSS_COMPILE}-gcc CPP=${PROJECT_CROSS_COMPILE}-g++ ./Configure --prefix=${OUT_3RD_DIR}/usr shared linux-generic32 no-asm no-async
	fi
	cd ${DST_FILE}
	make
	make install
	
	unset CC
	unset CPP
	
	build_lib_end
}

make_openssl