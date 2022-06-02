// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

#include "NullLogger.h"
#include "LogLevel.h"
#include "Logger.h"

namespace Atone {

    class Log {

        private:
            static std::shared_ptr<Logger> logger;

        public:
            static void set(const std::shared_ptr<Logger> &logger);

            template <typename... Args>
            inline static void emergency(const char *format, Args... args) {
                logger->Log(LogLevel::Emergency, format, args...);
            }

            template <typename... Args>
            inline static void fatal(const char *format, Args... args) {
                logger->Log(LogLevel::Fatal, format, args...);
            }

            template <typename... Args>
            inline static void crit(const char *format, Args... args) {
                logger->Log(LogLevel::Critical, format, args...);
            }

            template <typename... Args>
            inline static void error(const char *format, Args... args) {
                logger->Log(LogLevel::Error, format, args...);
            }

            template <typename... Args>
            inline static void warn(const char *format, Args... args) {
                logger->Log(LogLevel::Warning, format, args...);
            }

            template <typename... Args>
            inline static void notice(const char *format, Args... args) {
                logger->Log(LogLevel::Notice, format, args...);
            }

            template <typename... Args>
            inline static void info(const char *format, Args... args) {
                logger->Log(LogLevel::Information, format, args...);
            }

            template <typename... Args>
            inline static void debug(const char *format, Args... args) {
                logger->Log(LogLevel::Debug, format, args...);
            }

            template <typename... Args>
            inline static void trace(const char *format, Args... args) {
                logger->Log(LogLevel::Trace, format, args...);
            }

    };

}