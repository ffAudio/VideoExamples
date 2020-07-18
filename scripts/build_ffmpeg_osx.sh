#!/bin/bash

pushd ../libraries
mkdir build
pushd build

# ../ffmpeg/configure --enable-shared --disable-static --enable-gpl --enable-libx264 --install-name-dir='@executable_path/../Resources/ffmpeg_libs' --prefix=`pwd` --libdir=`pwd`/ffmpeg_libs
../ffmpeg/configure --enable-shared --disable-static --enable-gpl --enable-libx264 --install-name-dir='@executable_path/../Resources/ffmpeg_libs' --prefix=`pwd`
make -j 8
make install

popd
popd
