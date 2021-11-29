#!/bin/bash

function _cmd_get_git_version_number()
{
	GIT_COMMIT=$(git log |grep commit -m 1|awk '{print $2 }')
	GIT_COMMIT_AUTH=$(git log |grep Author: -m 1|awk '{print $2}')
	GIT_LOG_DATE=$(git log |grep Date: -m 1|awk '{print $2 " " $3 " " $4 " " $5 " " $6 " " $7}')
	export GIT_LOG_VERSION=${GIT_COMMIT}
	GIT_VERSION_FILE="${PROJECT_DIR}/app/appmain/lib/include/git_log_version_number.h"
	
	echo -e "/* This is auto make file! by build_cmd.sh/cmd_get_git_version_number*/\r\n" > ${GIT_VERSION_FILE}
	echo -e "#ifndef __GIT_VERSION_NUMBER_H__" >> ${GIT_VERSION_FILE}
	echo -e "#define __GIT_VERSION_NUMBER_H__\r\n"  >> ${GIT_VERSION_FILE}
	echo -e "#define GIT_COMMIT 	\"${GIT_COMMIT}\"  \r\n" >> ${GIT_VERSION_FILE}
	echo -e "#define GIT_LOG_DATE 		\"${GIT_LOG_DATE}\"  \r\n" >> ${GIT_VERSION_FILE}
	echo -e "#define GIT_COMMIT_AUTH 	\"${GIT_COMMIT_AUTH}\"  \r\n" >> ${GIT_VERSION_FILE}
	echo -e "#endif\r\n"  >> ${GIT_VERSION_FILE}
}

function cmd_get_git_version_number()
{
	pushd ${PROJECT_DIR}/app/appmain;_cmd_get_git_version_number;popd;
}


