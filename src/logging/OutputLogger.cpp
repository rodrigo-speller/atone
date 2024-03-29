// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "OutputLogger.h"

#include "LogLevel.h"
#include "Logger.h"

namespace Atone {

    OutputLogger::OutputLogger() {
        this->minimumLevel = LogLevel::Information;
    }

    OutputLogger::OutputLogger(LogLevel minimumLevel) {
        this->minimumLevel = minimumLevel;
    }

    void OutputLogger::Log(LogLevel level, const char *format, ...) {
        if (minimumLevel > level)
            return;

        begin_log(level);

        va_list list;
        va_start(list, format);
        vprintf(format, list);
        printf("\n");
        va_end(list);

        end_log();
    }

    void OutputLogger::begin_log(LogLevel level) {
        print_time();
        printf(" ");
        print_level(level);
        printf(" atone: ");
    }

    void OutputLogger::print_time()
    {
        timespec time;
        clock_gettime(CLOCK_REALTIME, &time);

        struct tm *timeinfo;
        timeinfo = localtime(&time.tv_sec);
        printf(
            "[%i-%02i-%02i %02i:%02i:%02i.%03i]",
            timeinfo->tm_year + 1900,
            timeinfo->tm_mon + 1,
            timeinfo->tm_mday,
            timeinfo->tm_hour,
            timeinfo->tm_min,
            timeinfo->tm_sec,
            (int)time.tv_nsec / 1000000
        );
    }

    void OutputLogger::print_level(LogLevel level)
    {
        switch (level) {
            case LogLevel::Emergency:
                printf("EMERG");
                return;
            case LogLevel::Fatal:
                printf("FATAL");
                return;
            case LogLevel::Critical:
                printf("CRIT ");
                return;
            case LogLevel::Error:
                printf("ERROR");
                return;
            case LogLevel::Warning:
                printf("WARN ");
                return;
            case LogLevel::Notice:
                printf("NOTIC");
                return;
            case LogLevel::Information:
                printf("INFO ");
                return;
            case LogLevel::Debug:
                printf("DEBUG");
                return;
            case LogLevel::Trace:
                printf("TRACE");
                return;
            default:
                break;
        }
    }

    void OutputLogger::end_log() {
        fflush(stdout);
    }

}