#!/bin/bash -x

set -e

build_platformio() {
    pushd $1
    platformio run
    popd
}

build_cmake() {
    pushd $1
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(grep -c ^processor /proc/cpuinfo)
    popd
}

if [[ "$1" == "clean" ]]; then
   rm -rf */.pio */build
   exit
fi

build_platformio blinken-sort-esp8266
build_platformio blinken-sort-teensy
build_cmake blinken-sort-pi
build_cmake blinken-sort-sound-pi

build_platformio random-flux-esp8266
build_platformio random-flux-teensy
build_cmake random-flux-pi
