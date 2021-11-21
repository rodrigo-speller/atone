// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "LogLevel.h"
#include "Logger.h"

namespace Atone {

    class TerminalLogger : public Logger {
        public:
            TerminalLogger();
            TerminalLogger(LogLevel minimumLevel);

            void Log(LogLevel level, const char *format, ...);

        private:
            LogLevel minimumLevel;

            void begin_log(LogLevel level);
            void print_time();
            void print_level(LogLevel level);
            void end_log();
    };

}