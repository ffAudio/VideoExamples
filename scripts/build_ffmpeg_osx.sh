#!/bin/bash

pushd ../libraries
mkdir build
pushd build

../ffmpeg/configure --enable-shared --enable-gpl --enable-libx264 --install-name-dir='@executable_path' --prefix=`pwd` --libdir=`pwd`/ffmpeg_libs
make -j 8
make install

popd
popd
