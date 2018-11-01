/*******************************************************************************
 * lib/NeoAnimation/Extra/MAX7219.hpp
 *
 * Class to write pixels to a MAX7219 dot matrix LED array via SPI from a
 * Raspberry Pi.
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_EXTRA_MAX7219_HEADER
#define NEOANIMATION_EXTRA_MAX7219_HEADER

#include <cerrno>
#include <cstring>
#include <iostream>
#include <vector>

#include <asm/ioctl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

#include <NeoAnimation/Extra/PiGPIO.hpp>

namespace NeoAnimation {

class MAX7219
{
public:
    // max7219 registers
    static const uint8_t MAX7219_REG_NOOP = 0x0;
    static const uint8_t MAX7219_REG_DIGIT0 = 0x1;
    static const uint8_t MAX7219_REG_DIGIT1 = 0x2;
    static const uint8_t MAX7219_REG_DIGIT2 = 0x3;
    static const uint8_t MAX7219_REG_DIGIT3 = 0x4;
    static const uint8_t MAX7219_REG_DIGIT4 = 0x5;
    static const uint8_t MAX7219_REG_DIGIT5 = 0x6;
    static const uint8_t MAX7219_REG_DIGIT6 = 0x7;
    static const uint8_t MAX7219_REG_DIGIT7 = 0x8;
    static const uint8_t MAX7219_REG_DECODEMODE = 0x9;
    static const uint8_t MAX7219_REG_INTENSITY = 0xA;
    static const uint8_t MAX7219_REG_SCANLIMIT = 0xB;
    static const uint8_t MAX7219_REG_SHUTDOWN = 0xC;
    static const uint8_t MAX7219_REG_DISPLAYTEST = 0xF;

    MAX7219(std::string path, size_t cs_pin) {
        memset(display_, 0, sizeof(display_));

        fd_ = open(path.c_str(), O_RDWR);
        if (fd_ < 0) {
            std::cerr << "MAX7219 failed" << std::endl;
            return;
        }

        int mode = 3;
        if (ioctl(fd_, SPI_IOC_WR_MODE, &mode) < 0) {
            std::cerr << "SPI Mode Change failure: "
                      << strerror(errno) << std::endl;
        }

        int spiBPW = 8;
        if (ioctl(fd_, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0) {
            std::cerr << "SPI BPW Change failure: "
                      << strerror(errno) << std::endl;
        }

        spiSpeed_ = 1200000;
        if (ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed_) < 0) {
            std::cerr << "SPI Speed Change failure: "
                      << strerror(errno) << std::endl;
        }

        cs_gpio_.set_pin(cs_pin, /* output */ true);
        cs_gpio_.write(0);
    }

    void PutPixel(size_t x, size_t y, bool on) {
        if (x >= 8 * 8)
            return;

        if (y < 8) {
            if (on)
                display_[(7 - x / 8) * 8 + (7 - y)] |= 1 << (x % 8);
            else
                display_[(7 - x / 8) * 8 + (7 - y)] &= ~(1 << (x % 8));
        }
        else if (y < 16) {
            y -= 8;
            if (on)
                display_[8 * 8 + (7 - x / 8) * 8 + (7 - y)] |= 1 << (x % 8);
            else
                display_[8 * 8 + (7 - x / 8) * 8 + (7 - y)] &= ~(1 << (x % 8));
        }
    }

    void clear() {
        memset(display_, 0, sizeof(display_));
    }

    void show() {
        // show all 8 digits
        Broadcast(MAX7219_REG_SCANLIMIT, 7);
        // using an led matrix (not digits)
        Broadcast(MAX7219_REG_DECODEMODE, 0);
        // no display test
        Broadcast(MAX7219_REG_DISPLAYTEST, 0);
        // character intensity: range: 0 to 15
        Broadcast(MAX7219_REG_INTENSITY, 15);
        // not in shutdown mode (ie. start it up)
        Broadcast(MAX7219_REG_SHUTDOWN, 1);

        for (size_t j = 0; j < 8; ++j) {
            cs_gpio_.write(0);
            for (size_t i = 0; i < num_devices_; ++i) {
                SPIwrite_pair(MAX7219_REG_DIGIT0 + j, display_[8 * i + j]);
            }
            cs_gpio_.write(1);
        }
    }

protected:
    static const size_t num_devices_ = 16;

    uint8_t display_[num_devices_ * 8];

    void Broadcast(uint8_t reg, uint8_t data) {
        cs_gpio_.write(0);
        for (size_t i = 0; i < num_devices_; ++i) {
            SPIwrite_pair(reg, data);
        }
        cs_gpio_.write(1);
    }

    void SPIwrite_pair(uint8_t reg, uint8_t data) {
        uint8_t a[2] = { reg, data };
        SPIwrite(a, sizeof(a));
    }

    int SPIwrite(unsigned char* data, int len) {

        struct spi_ioc_transfer spi;
        memset(&spi, 0, sizeof(spi));

        spi.tx_buf = (unsigned long)data;
        spi.rx_buf = 0x0;
        spi.len = len;
        spi.delay_usecs = 0;
        spi.speed_hz = spiSpeed_;
        spi.bits_per_word = 8;

        int x = ioctl(fd_, SPI_IOC_MESSAGE(1), &spi);

        return x;
    }

private:
    //! strip length
    size_t strip_size_;

    //! device file descriptor
    int fd_;

    //! SPI speed
    size_t spiSpeed_;

    //! GPIO CS Pin for SPI multiplex
    GPIOPin cs_gpio_;
};

} // namespace NeoAnimation

#endif // !NEOANIMATION_EXTRA_MAX7219_HEADER

/******************************************************************************/
