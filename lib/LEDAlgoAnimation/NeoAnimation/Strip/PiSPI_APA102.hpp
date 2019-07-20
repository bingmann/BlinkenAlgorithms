/*******************************************************************************
 * lib/NeoAnimation/Strip/PiSPI_APA102.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_STRIP_PISPI_APA102_HEADER
#define NEOANIMATION_STRIP_PISPI_APA102_HEADER

#include <NeoAnimation/Color.hpp>
#include <NeoAnimation/Extra/PiGPIO.hpp>
#include <NeoAnimation/Strip/LEDStripBase.hpp>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <vector>

#include <asm/ioctl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

namespace NeoAnimation {

class PiSPI_APA102 : public LEDStripBase
{
public:
    PiSPI_APA102(std::string path, size_t strip_size, int cs_pin = -1)
        : strip_size_(strip_size),
          strip_data_(strip_size) {

        fd_ = open(path.c_str(), O_RDWR);
        if (fd_ < 0) {
            std::cerr << "PiSPI_APA102 failed" << std::endl;
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

        spiSpeed_ = 9600000;
        if (ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed_) < 0) {
            std::cerr << "SPI Speed Change failure: "
                      << strerror(errno) << std::endl;
        }

        if (cs_pin >= 0) {
            cs_gpio_.set_pin(cs_pin, /* output */ true);
            cs_gpio_.write(0);
        }
    }

    struct APAColor {
        uint8_t w = 0, b = 0, g = 0, r = 0;
    };

    void setPixel(size_t index, const Color& color) {
        if (index < strip_size_) {
            // try to transform color to RGB + brightness
            unsigned r = color.r, g = color.g, b = color.b;
            r += color.w, g += color.w, b += color.w;
            r = r > 255 ? 255 : r, g = g > 255 ? 255 : g, b = b > 255 ? 255 : b;
            unsigned m = std::max(std::max(r, g), b);
            strip_data_[index].r = r;
            strip_data_[index].g = g;
            strip_data_[index].b = b;
            strip_data_[index].w = 0b11100000 | (0b00011111 & (m >> 3));
        }
    }

    void orPixel(size_t index, const Color& color) {
        if (index < strip_size_) {
            // try to transform color to RGB + brightness
            unsigned r = color.r, g = color.g, b = color.b;
            r += color.w, g += color.w, b += color.w;
            r = r > 255 ? 255 : r, g = g > 255 ? 255 : g, b = b > 255 ? 255 : b;
            unsigned m = std::max(std::max(r, g), b);
            strip_data_[index].r |= r;
            strip_data_[index].g |= g;
            strip_data_[index].b |= b;
            strip_data_[index].w |= 0b11100000 | (0b00011111 & (m >> 3));
        }
    }

    bool busy() const { return false; }

    void show() {
        uint8_t buf_start[4] = { 0x00, 0x00, 0x00, 0x00 };
        SPIwrite(buf_start, 4);

        SPIwrite(reinterpret_cast<uint8_t*>(strip_data_.data()),
                 4 * strip_size_);

        buf_start[0] = 0xFF;
        for (size_t i = 0; i < (strip_size_ / 2 + 7) / 8; ++i) {
            SPIwrite(buf_start, 1);
        }
    }

    size_t size() const { return strip_size_; }

protected:
    int SPIwrite(unsigned char* data, int len) {

        cs_gpio_.write(1);

        struct spi_ioc_transfer spi;
        memset(&spi, 0, sizeof(spi));

        spi.tx_buf = (unsigned long)data;
        spi.rx_buf = 0x0;
        spi.len = len;
        spi.delay_usecs = 0;
        spi.speed_hz = spiSpeed_;
        spi.bits_per_word = 8;

        int x = ioctl(fd_, SPI_IOC_MESSAGE(1), &spi);

        cs_gpio_.write(1);

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

    //! strip color data
    std::vector<APAColor> strip_data_;
};

} // namespace NeoAnimation

#endif // !NEOANIMATION_STRIP_PISPI_APA102_HEADER

/******************************************************************************/
