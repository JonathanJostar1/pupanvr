#!/bin/bash


function make_png()
{
	ECHO ""
	ECHO "=============build libpng-1.6.34============="
	
	SRC_FILE=${PROJECT_DIR}/third_party/tarball/libpng-1.6.34.tar.xz
	DST_FILE=${OUT_3RD_DIR}/obj/libpng-1.6.34
	
	ECHO ${SRC_FILE}
	
	if [ ! -e ${SRC_FILE} ]
	then
		ERR "not find ${SRC_FILE}"
		exit -1
	fi
	
	if [ ! -d ${DST_FILE} ]
	then
		tar -Jxf ${SRC_FILE} -C ${OUT_3RD_DIR}/obj
		cd ${DST_FILE}
		chmod u+x configure
		cp ${OUT_3RD_DIR}/usr/include/*.h ./
		LDFLAGS="-L${OUT_3RD_DIR}/usr/lib" LIBS="-lz" CFLAGS="-I${OUT_3RD_DIR}/usr/include" ./configure --prefix=${OUT_3RD_DIR}/usr --enable-static --host=${PROJECT_CROSS_COMPILE}
	fi

	cd ${DST_FILE}
	make
	make install
	build_lib_end
}

make_png