// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "time.h"

namespace Atone {
    /**
     * Compute the timeout based on the given time.
     * 
     * @param time The time to use.
     * @return 0 on success, -1 on error.
     */
    int timeout_from_now(struct timespec &time) {
        timespec now;

        if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
            return -1;
        }

        return timespec_add(now, time, time);
    }

    /**
     * Check if the timeout has expired.
     * 
     * @param time The timeout.
     * @return Returns true if the timeout has expired, otherwise false.
     */
    bool timeout_expired(const struct timespec &time) {
        timespec now;

        if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
            return -1;
        }

        return timespec_cmp(now, time) >= 0;
    }

    /**
     * Compares two timespecs.
     * 
     * @param a The first timespec.
     * @param b The second timespec.
     * @return Returns -1 if a < b, 0 if a == b, 1 if a > b.
     */
    int timespec_cmp(const struct timespec &a, const struct timespec &b) {
        if (a.tv_sec < b.tv_sec) {
            return -1;
        } else if (a.tv_sec > b.tv_sec) {
            return 1;
        } else {
            if (a.tv_nsec < b.tv_nsec) {
                return -1;
            } else if (a.tv_nsec > b.tv_nsec) {
                return 1;
            } else {
                return 0;
            }
        }
    }

    /**
     * Adds two timespec structures.
     *
     * @param time1 The first timespec structure.
     * @param time2 The second timespec structure.
     * @param result The result of the addition.
     *
     * @return 0 on success, -1 on error.
     */
    int timespec_add(const struct timespec &time1, const struct timespec &time2, struct timespec &result) {
        result.tv_sec = time1.tv_sec + time2.tv_sec;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec;

        if (result.tv_nsec >= 1000000000L) {
            result.tv_sec++;
            result.tv_nsec -= 1000000000UL;
        }

        return 0;
    }

    /**
     * Subtracts two timespec structures.
     *
     * @param time1 The first timespec structure.
     * @param time2 The second timespec structure.
     * @param result The result of the subtraction.
     *
     * @return 0 on success, -1 on error.
     */
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