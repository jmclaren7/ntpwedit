#!/bin/sh

BUILD_TYPE='Release'
CMAKE='cmake -D CMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw64.cmake'
BUILD_DIR='build'

test -d $BUILD_DIR || mkdir $BUILD_DIR
cd $BUILD_DIR || exit

$CMAKE -D CMAKE_BUILD_TYPE=${BUILD_TYPE} ..
