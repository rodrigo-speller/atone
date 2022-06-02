// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "LogLevel.h"
#include "Logger.h"

namespace Atone {

    class NullLogger : public Logger {
        public:
            void Log(LogLevel level, const char *format, ...);
    };

}