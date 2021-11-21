// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "LogLevel.h"

namespace Atone {

    class Logger {
        public:
            virtual void Log(LogLevel level, const char *format, ...) = 0;
    };

}