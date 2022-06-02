// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "SyslogLogger.h"

namespace Atone {

    void SyslogLogger::Log(LogLevel level, const char *format, ...) {
        int syslevel = LOG_INFO;

        switch (level)
        {
            case LogLevel::Trace:
            case LogLevel::Debug:
                syslevel = LOG_DEBUG;
                break;
            case LogLevel::Information:
                syslevel = LOG_INFO;
                break;
            case LogLevel::Notice:
                syslevel = LOG_NOTICE;
                break;
            case LogLevel::Warning:
                syslevel = LOG_WARNING;
                break;
            case LogLevel::Error:
                syslevel = LOG_ERR;
                break;
            case LogLevel::Critical:
                syslevel = LOG_CRIT;
                break;
            case LogLevel::Fatal:
                syslevel = LOG_ALERT;
                break;
            case LogLevel::Emergency:
                syslevel = LOG_EMERG;
                break;
            default:
                throw std::domain_error("invlaid log level");
        }

        va_list list;
        va_start(list, format);
        vsyslog(syslevel, format, list);
        va_end(list);
    }

}