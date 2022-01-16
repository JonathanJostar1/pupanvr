#!/bin/bash

source ${PROJECT_CONFIG_DIR}/script/common.sh

OUT_3RD_DIR=${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/third_party

[ -d ${OUT_3RD_DIR}/obj/ ] || mkdir -p ${OUT_3RD_DIR}/obj/
[ -d ${OUT_3RD_DIR}/usr/ ] || mkdir -p ${OUT_3RD_DIR}/usr/

[ -d ${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/${PRODUCT_MODEL}/third_party/bin ] || mkdir -p ${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/${PRODUCT_MODEL}/third_party/bin

#脚本执行完后的收尾工作
function build_lib_end()
{
	cd ${PROJECT_DIR}/third_party/${DEV_SDK}
}

function pack_lib_install()
{
	#使用动态编译时为动态库打包
	OUT_LIB_PACK_DIR=${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/${PRODUCT_MODEL}/install/lib/
	[ -d ${OUT_LIB_PACK_DIR} ] || mkdir -p ${OUT_LIB_PACK_DIR}
	#TO-DO
}

#source		glog/build.sh
#source		iniparser/build.sh
#source		zlib/build_zlib.sh
#source 	libpng/build_png.sh
source 		harfbuzz/buid_harfbuzz.sh
#source		freetype/build_libfreetype.sh
#source		libjpeg/build_libjpeg.sh