#!/bin/bash -x
################################################################################
# misc/compile-test.sh
#
# Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
#
# All rights reserved. Published under the GNU General Public License v3.0
################################################################################

set -e

for p in *-esp8266 *-teensy; do
    pushd $p
    rm -rf .pio
    platformio run
    popd
done

for p in *-pi; do
    pushd $p
    rm -rf b
    [ -e b ] || (mkdir b && cd b && cmake .. && cd ..)
    make -C b -j4
    popd;
done

################################################################################
