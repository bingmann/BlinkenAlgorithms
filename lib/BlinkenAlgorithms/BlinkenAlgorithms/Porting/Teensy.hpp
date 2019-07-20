/*******************************************************************************
 * lib/BlinkenAlgorithms/BlinkenAlgorithms/Porting/Teensy.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef BLINKENALGORITHMS_PORTING_TEENSY_HEADER
#define BLINKENALGORITHMS_PORTING_TEENSY_HEADER

/******************************************************************************/

// // add missing symbols
// extern "C" {
// int _getpid() {
//     return -1;
// }
// int _kill(int pid, int sig) {
//     return -1;
// }
// } // extern "C"

/******************************************************************************/

// fix problem with Teensy's compile chain and <random>
#if TEENSYDUINO
namespace std {

double max(float a, double b) {
    return std::max(static_cast<double>(a), b);
}

} // namespace std
#endif

#include <random>

/******************************************************************************/

#endif // !BLINKENALGORITHMS_PORTING_TEENSY_HEADER

/******************************************************************************/
