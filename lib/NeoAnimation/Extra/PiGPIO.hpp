/*******************************************************************************
 * lib/NeoAnimation/Extra/PiGPIO.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_EXTRA_PIGPIO_HEADER
#define NEOANIMATION_EXTRA_PIGPIO_HEADER

#include <fcntl.h>
#include <unistd.h>

namespace NeoAnimation {

class GPIOPin
{
public:
    //! construct null pin
    GPIOPin() = default;

    //! construct and initialize
    GPIOPin(int pin, bool output) {
        set_pin(pin, output);
    }

    ~GPIOPin() {
        if (pin_ >= 0)
            unexport_pin(pin_);
    }

    //! initialize
    bool set_pin(int pin, bool output) {
        if (export_pin(pin) && set_direction(pin, output)) {
            pin_ = pin;
            return true;
        }
        return false;
    }

    //! return initialized pin
    int pin() const { return pin_; }

    //! read value of GPIO pin
    int read() {
        if (pin_ < 0)
            return -1;

        char value_str[3];

        if (::read(fd_value_, value_str, 3) < 0) {
            fprintf(stderr, "Failed to read value!\n");
            return -1;
        }

        return atoi(value_str);
    }

    //! set value of GPIO pin
    void write(bool value) {
        if (pin_ < 0)
            return;

        static const char* s_values_str[] = { "0\n", "1\n" };

        if (::write(fd_value_, s_values_str[value], 2) != 2) {
            fprintf(stderr, "Failed to write value!\n");
            return;
        }
    }

protected:
    static bool export_pin(int pin) {
        char buffer[16];

        int fd = open("/sys/class/gpio/export", O_WRONLY);
        if (fd < 0) {
            fprintf(stderr, "Failed to open export for writing!\n");
            return false;
        }

        ssize_t bytes_written = snprintf(buffer, sizeof(buffer), "%d", pin);
        ::write(fd, buffer, bytes_written);
        close(fd);

        return true;
    }

    static bool unexport_pin(int pin) {
        char buffer[16];

        int fd = open("/sys/class/gpio/unexport", O_WRONLY);
        if (fd < 0) {
            fprintf(stderr, "Failed to open unexport for writing!\n");
            return false;
        }

        ssize_t bytes_written = snprintf(buffer, sizeof(buffer), "%d", pin);
        ::write(fd, buffer, bytes_written);
        close(fd);

        return true;
    }

    //! Set GPIO pin direction: false = INPUT, true = OUTPUT
    bool set_direction(int pin, bool output) {

        static const char* s_directions_str[2] = { "in\n", "out\n" };
        char path[48];

        snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);

        for (size_t r = 0; ; ++r) {
            int fd = open(path, O_WRONLY);
            if (fd < 0) {
                if (r >= 50) {
                    fprintf(stderr, "Failed to open gpio direction for writing!\n");
                    return false;
                }
                usleep(5000);
                continue;
            }

            if (::write(fd, s_directions_str[output], output ? 4 : 3) < 0) {
                if (r >= 50) {
                    fprintf(stderr, "Failed to set direction!\n");
                    return false;
                }
                usleep(5000);
                continue;
            }

            close(fd);
            break;
        }

        // open value fd
        snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);

        if (!output) {
            fd_value_ = open(path, O_RDONLY);
            if (fd_value_ < 0) {
                fprintf(stderr, "Failed to open gpio value for reading!\n");
                return false;
            }
        }
        else {
            fd_value_ = open(path, O_WRONLY);
            if (fd_value_ < 0) {
                fprintf(stderr, "Failed to open gpio value for writing!\n");
                return false;
            }
        }

        return true;
    }

protected:
    //! pin number
    int pin_ = -1;

    //! fd for value
    int fd_value_;
};

} // namespace NeoAnimation

#endif // !NEOANIMATION_EXTRA_PIGPIO_HEADER

/******************************************************************************/
