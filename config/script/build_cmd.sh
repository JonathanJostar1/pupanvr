#!/bin/bash

source ${PROJECT_DIR}/config/script/cmd_get_version.sh

function cmd_mk_chip_out_dir()
{
	echo ${PROJECT_DIR} ${DEV_CHIP}
	#make chip dir
	[ -d ${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/${PRODUCT_MODEL} ] || mkdir -p ${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/${PRODUCT_MODEL}
	
	[ -d ${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/third_party ] || mkdir -p ${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/third_party
	
	[ -d ${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/${PRODUCT_MODEL}/app/build/bin ] || mkdir -p ${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/${PRODUCT_MODEL}/source/build/bin
	
	[ -d ${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/${PRODUCT_MODEL}/install ] || mkdir -p ${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/${PRODUCT_MODEL}/install
}


function cmd_make()
{
	cd ${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/${PRODUCT_MODEL}/app/build
	cmake ${PROJECT_DIR}/app -DCROSS_COMPILE="${PROJECT_CROSS_COMPILE}" -DCHIP="${DEV_CHIP}" \
	-DPROJECT_DIR="${PROJECT_DIR}" -DUSER_CFLAGS="${PROJECT_CFLAGS}" \
	-DINSTALL_DIR="${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/${PRODUCT_MODEL}/install" \
	-DBOARD_SDK_DIR="${PROJECT_SDK_DIR}" -DTHIRD_PARTY_DIR="${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/third_party"\
	-DBOARD_BSP_DIR="${PROJECT_BSP_DIR}" -DSVN_VERSION_NUMBER="${SVN_VERSION_NUMBER}" \
	-DPRODUCT_MODEL=${PRODUCT_MODEL}\
	-DBOARD="${DEV_CHIP}" \
	-DOS="${DEV_OS}"
}

function cmd_make_install()
{
	cd ${PROJECT_BUILDOUT_DIR}/${DEV_CHIP}/${PRODUCT_MODEL}/app/build
	make install
}


cmd_mk_chip_out_dir
cmd_get_git_version_number
cmd_make
