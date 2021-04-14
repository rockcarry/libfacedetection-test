#!/bin/sh

set -e

TOPDIR=$PWD

if [ ! -e $TOPDIR/libfacedetection ]; then
    git clone https://github.com/ShiqiYu/libfacedetection.git
fi

case "$1" in
"")
    mkdir -p $TOPDIR/libfacedetection/build && cd $TOPDIR/libfacedetection/build
    cmake .. \
    -DCMAKE_INSTALL_PREFIX=$TOPDIR/libfacedetection/install \
    -DCMAKE_BUILD_TYPE=Release \
    -DUSE_OPENMP=OFF \
    -DBUILD_SHARED_LIBS=OFF \
    -DENABLE_AVX2=OFF \
    -DENABLE_AVX512=OFF \
    -DDEMO=OFF
    make -j8 && make install
    cd $TOPDIR
    g++ -I$TOPDIR/libfacedetection/install/include/facedetection \
        -L$TOPDIR/libfacedetection/install/lib \
        -Wall bmpfile.c test.c -lfacedetection -o test
    ;;
clean)
    rm -rf $TOPDIR/libfacedetection/build $TOPDIR/libfacedetection/install
    rm -rf test
    ;;
esac
