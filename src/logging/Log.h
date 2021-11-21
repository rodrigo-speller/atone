// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <cstdarg>

#include "NullLogger.h"
#include "LogLevel.h"
#include "Logger.h"

namespace Atone {

    class Log {

        private:
            static Logger *logger;

        public:
            static void set(Logger *logger);


            template <typename... Args>
            static void emergency(const char *format, Args... args) {
                logger->Log(LogLevel::Emergency, format, args...);
            }

            template <typename... Args>
            static void fatal(const char *format, Args... args) {
                logger->Log(LogLevel::Fatal, format, args...);
            }

            template <typename... Args>
            static void crit(const char *format, Args... args) {
                logger->Log(LogLevel::Critical, format, args...);
            }

            template <typename... Args>
            static void error(const char *format, Args... args) {
                logger->Log(LogLevel::Error, format, args...);
            }

            template <typename... Args>
            static void warn(const char *format, Args... args) {
                logger->Log(LogLevel::Warning, format, args...);
            }

            template <typename... Args>
            static void notice(const char *format, Args... args) {
                logger->Log(LogLevel::Notice, format, args...);
            }

            template <typename... Args>
            static void info(const char *format, Args... args) {
                logger->Log(LogLevel::Information, format, args...);
            }

            template <typename... Args>
            static void debug(const char *format, Args... args) {
                logger->Log(LogLevel::Debug, format, args...);
            }

            template <typename... Args>
            static void trace(const char *format, Args... args) {
                logger->Log(LogLevel::Trace, format, args...);
            }

    };

}