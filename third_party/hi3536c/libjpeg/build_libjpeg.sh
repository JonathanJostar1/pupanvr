#!/bin/bash

function make_jpegv6b()
{
	ECHO ""
	ECHO "=============build jpegsrc.v9c.tar.gz============="
	
	SRC_FILE=${PROJECT_DIR}/third_party/tarball/jpegsrc.v9c.tar.gz
	DST_FILE=${OUT_3RD_DIR}/obj/jpeg-9c
	
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
		CC=${PROJECT_CROSS_COMPILE}-gcc LD=${PROJECT_CROSS_COMPILE}-ld LIBTOOL="/usr/bin/libtool" ./configure --host=${PROJECT_CROSS_COMPILE} --prefix=${OUT_3RD_DIR}/usr --enable-shared 
	fi
	cd ${DST_FILE}
	cp /usr/bin/libtool ./
	make LIBTOOL="/usr/bin/libtool"
	
	if [ ! -d ${OUT_3RD_DIR}/usr/man/man1 ]
	then
		mkdir -p ${OUT_3RD_DIR}/usr/man/man1
	fi
	make install	

	build_lib_end
}

function make_jpegv9c()
{
	ECHO ""
	ECHO "=============build jpegsrc.v9c.tar.gz============="
	
	SRC_FILE=${PROJECT_DIR}/third_party/tarball/jpegsrc.v9c.tar.gz
	DST_FILE=${OUT_3RD_DIR}/obj/jpeg-9c
	
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
		./configure --host=${PROJECT_CROSS_COMPILE} --prefix=${OUT_3RD_DIR}/usr --enable-shared 
	fi
	make
	make install	

	build_lib_end
}


make_jpegv9c
