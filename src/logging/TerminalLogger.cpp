// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "TerminalLogger.h"

#include <cstdarg>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "LogLevel.h"
#include "Logger.h"
#include "utils/constants.h"

namespace Atone {

    TerminalLogger::TerminalLogger() {
        this->minimumLevel = LogLevel::Trace;
    }

    TerminalLogger::TerminalLogger(LogLevel minimumLevel) {
        this->minimumLevel = minimumLevel;
    }

    void TerminalLogger::Log(LogLevel level, const char *format, ...) {
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

    void TerminalLogger::begin_log(LogLevel level) {
        printf(ATONE_TERM_CSI_RESET);
        switch (level) {
            case LogLevel::Emergency:
            case LogLevel::Fatal:
            case LogLevel::Critical:
            case LogLevel::Error:
                printf(ATONE_TERM_CSI_FOREGROUND_RED);
                break;
            case LogLevel::Warning:
                printf(ATONE_TERM_CSI_FOREGROUND_YELLOW_LIGHT);
                break;
            case LogLevel::Notice:
            case LogLevel::Information:
                printf(ATONE_TERM_CSI_FOREGROUND_WHITE_LIGHT);
                break;
            case LogLevel::Debug:
                printf(ATONE_TERM_CSI_FOREGROUND_CYAN_LIGHT);
                break;
            case LogLevel::Trace:
                printf(ATONE_TERM_CSI_FOREGROUND_GREEN_LIGHT);
                break;
            default:
                break;
        }

        print_time();
        printf(" ");
        print_level(level);
        printf(" atone: ");
    }

    void TerminalLogger::print_time()
    {
        timeval tv;
        gettimeofday(&tv, 0);

        time_t rawtime = tv.tv_sec;
        struct tm *timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf(
            "[%i-%02i-%02i %02i:%02i:%02i.%03i]",
            timeinfo->tm_year + 1900,
            timeinfo->tm_mon,
            timeinfo->tm_mday,
            timeinfo->tm_hour,
            timeinfo->tm_min,
            timeinfo->tm_sec,
            (int)tv.tv_usec / 1000
        );
    }

    void TerminalLogger::print_level(LogLevel level)
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

    void TerminalLogger::end_log() {
        printf(ATONE_TERM_CSI_RESET);
        fflush(stdout);
    }

}