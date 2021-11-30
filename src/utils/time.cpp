// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "time.h"
#include <errno.h>

namespace Atone {
    
    int timeout_from_now(struct timespec &time) {
        timespec now;

        if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
            return -1;
        }

        return timespec_add(now, time, time);
    }

    int timespec_add(const struct timespec &time1, const struct timespec &time2, struct timespec &result) {
        result.tv_sec = time1.tv_sec + time2.tv_sec;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec;

        if (result.tv_nsec >= 1000000000L) {
            result.tv_sec++;
            result.tv_nsec -= 1000000000UL;
        }

        return 0;
    }

    int timespec_diff(const struct timespec &start, const struct timespec &stop, struct timespec &result) {

        if (start.tv_nsec > stop.tv_nsec) {
            result.tv_sec = stop.tv_sec - start.tv_sec - 1;
            result.tv_nsec = stop.tv_nsec - start.tv_nsec + 1000000000UL;
        }
        else {
            result.tv_sec = stop.tv_sec - start.tv_sec;
            result.tv_nsec = stop.tv_nsec - start.tv_nsec;
        }

        if (result.tv_sec < 0) {
            errno = EINVAL;
            return -1;
        }

        return 0;
    }

}