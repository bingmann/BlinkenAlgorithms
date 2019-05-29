/*******************************************************************************
 * lib/NeoAnimation/Porting/RaspberryPi.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_PORTING_RASPBERRYPI_HEADER
#define NEOANIMATION_PORTING_RASPBERRYPI_HEADER

#include <chrono>
#include <thread>

/******************************************************************************/

static inline void delay(size_t delay_time) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_time));
}

/******************************************************************************/

#endif // !NEOANIMATION_PORTING_RASPBERRYPI_HEADER

/******************************************************************************/
