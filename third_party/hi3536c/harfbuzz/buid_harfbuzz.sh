#!/bin/bash

function make_harfbuzz()
{
	ECHO ""
	ECHO "=============build harfbuzz-2.5.3.tar.xz============="
	
	SRC_FILE=${PROJECT_DIR}/third_party/tarball/harfbuzz-2.5.3.tar.xz
	DST_FILE=${OUT_3RD_DIR}/obj/harfbuzz-2.5.3
	
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
		FREETYPE_LIBS="-L${OUT_3RD_DIR}/usr/lib -lfreetype" FREETYPE_CFLAGS="-I${OUT_3RD_DIR}/usr/include/freetype2" ./configure --host=${PROJECT_CROSS_COMPILE} --prefix=${OUT_3RD_DIR}/usr --with-freetype=yes
	fi
	make -j4
	make install	

	build_lib_end
}


make_harfbuzz
