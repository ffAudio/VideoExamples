#!/bin/sh

FFMPEG_LIB=/usr/local/opt/ffmpeg/lib/
APP_BUNDLE=$1
APP_NAME=`echo $1 | sed 's/.*\///' | sed 's/.app$//'`

LIBS=('libavcodec.58.dylib' 'libavformat.58.dylib' 'libavresample.4.dylib' 'libavutil.56.dylib' 'libswresample.3.dylib' 'libswscale.5.dylib')

install_name_tool -add_rpath @executable_path/. ${APP_BUNDLE}/Contents/MacOS/${APP_NAME}

for lib in "${LIBS[@]}"; do
  cp ${FFMPEG_LIB}$lib ${APP_BUNDLE}/Contents/MacOS/
  install_name_tool -change ${FFMPEG_LIB}$lib @rpath/${lib} ${APP_BUNDLE}/Contents/MacOS/${APP_NAME}
done

