#!/bin/bash

C_COMPILER=gcc
CPP_COMPILER=g++
WARNING_FLAGS="-Wall -Wextra -pedantic"
C_FLAGS="$WARNING_FLAGS -O2 -Wl,-rpath,\$ORIGIN -pthread"
CPP_FLAGS="$C_FLAGS -std=c++11"
OPENCV_PATH="/opt/framos/libsv/opencv"
OPENCV_FLAGS="-isystem${OPENCV_PATH}/include -L${OPENCV_PATH}/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -Wl,-rpath,${OPENCV_PATH}/lib"
INCLUDE="-I../../include -I../. -L../../lib -lsv"

INCLUDE_ISP="-isystem/usr/src/jetson_multimedia_api/include"
LIB_ISP="-L/usr/lib/aarch64-linux-gnu/tegra/ -lnvbuf_utils -lv4l2"


BASEDIR=$(dirname "$0")
cd "$BASEDIR"
mkdir -p ../build
cd ../build

echo Building display_image cpp example...
$CPP_COMPILER ../../examples/display_image/display_image.cpp ../../examples/common_cpp/*.cpp -o display_image $CPP_FLAGS $OPENCV_FLAGS $INCLUDE