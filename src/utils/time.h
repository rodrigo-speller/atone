// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

namespace Atone {
    int timeout_from_now(struct timespec &time);
    
    int timespec_add(const struct timespec &time1, const struct timespec &time2, struct timespec &result);
    int timespec_diff(const struct timespec &start, const struct timespec &stop, struct timespec &result);
}
