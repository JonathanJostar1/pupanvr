#!/bin/bash

# for debug
DEBUG_LOG_FILE='&2'
DEBUG_LOG_LEVEL=0

# ANSI COLORS
COLOR_CRE="[K"
COLOR_NORMAL="[0;39m"
COLOR_RED="[1;31m"
COLOR_GREEN="[1;32m"
COLOR_YELLOW="[1;33m"
COLOR_BLUE="[1;34m"
COLOR_MAGENTA="[1;35m"
COLOR_CYAN="[1;36m"
COLOR_WHITE="[1;37m"


# Shell command
TAR=tar
CP=/bin/cp
RM=/bin/rm
GREP=grep
SED=sed
MKDIR=mkdir
CHMOD=chmod
MV=mv
CD=cd
LN=ln
MAKE=make
MKNOD=mknod
PUSHD=pushd 
POPD=popd 
RMDIR=rmdir
DEPMOD=/sbin/depmod
RMDIR=rmdir
MKIMG=mkimage
PATCH=patch
DIFF=diff
TOUCH=touch
CAT=cat

e_blank='[        ][      ]*'
e_year='20[0-9][0-9]'
e_month='([1-9]|0[1-9]|1[0-2])'
e_day='([1-9]|0[1-9]|[12][0-9]|3[0-1])'
e_time='([01][0-9]|2[0-3]):[0-5][0-9]'
e_employid='[a-zA-Z][a-zA-Z]*[0-9]{4,}'

#$1: string
#$2: color
ECHO()
{
	[ -n "$2" ] && eval echo -n \"\${${2}}\";
	echo "${1}${COLOR_NORMAL}"
}

ERR()
{
	echo "${COLOR_RED} ERR: ${1}${COLOR_NORMAL}" >&2
}

WARN()
{
	echo "${COLOR_YELLOW}WARN: ${1}${COLOR_NORMAL}" >&2
}

# $1:
LOG()
{
	echo "$1"
}


#$1: string
#$2: level
DEBUG()
{
	local level=$2
	[ -z "$level" ] && { level=0; }
	[ $level -lt $DEBUG_LOG_LEVEL ] && return 0;

	echo "$COLOR_WHITE$1$COLOR_NORMAL" > $DEBUG_LOG_FILE
}

#$1: path
abs_path()
{
	pushd "$1" >/dev/null
	[ $? -ne 0 ] && return 1;
	pwd
	popd >/dev/null
}


# $1: prefix
# $2..$n: dirs list
make_dirs()
{
	local make_dirs_count=2
	local make_dirs_dir=

	[ -z "$1" ] && { ERR "make_dirs mast have a prefix dir!"; return 1; }
	$MKDIR $1 -p

	while true
	do
		eval make_dirs_dir=\${$make_dirs_count}
		[ -z "$make_dirs_dir" ] && break;
		$MKDIR $1/$make_dirs_dir -p
		#((make_dirs_count++))
		((++make_dirs_count))
	done
}

check_dir_empty()
{
	[ -z "$1" ] && return 0;
	! [ -d $1 ] && return 0;
	[ -z "`find $1/ -maxdepth 1 -mindepth 1`" ] && return 0;

	return 1
}

# $1 - $2
# $3: frefix for '/', like "\\\\/"
sub_dir()
{
	local subdir=
	local dirA=`dirname $1/stub`
	local dirB=`dirname $2/stub`

	while [ "$dirA" != "$dirB" ] && [ "$dirA" != "." ] && [ "$dirA" != "/" ] 
	do
		if [ -z "$subdir" ] ; then
			subdir=`basename $dirA`
		else
			subdir=`basename $dirA`$3/$subdir
		fi
		dirA=`dirname $dirA`
	done

	[ -z "$subdir" ] && subdir=.

	dirname $subdir/stub
}


# $1: base dir
# $2: dest dir
# $3: frefix for '/', like "\\\\/"
base_offset_dir()
{
	local ofstdir=`sub_dir $2 $1`
	local bodofst=

	while [ "$ofstdir" != "." ] && [ "$ofstdir" != "/" ] 
	do
		if [ -z "$bodofst" ] ; then
			bodofst=..
		else
			bodofst=..$3/$bodofst
		fi
		ofstdir=`dirname $ofstdir`
	done

	dirname $bodofst/stub
}

