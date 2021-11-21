// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "Log.h"

#include <cstdarg>
#include <syslog.h>

#include "NullLogger.h"
#include "LogLevel.h"
#include "Logger.h"

namespace Atone {

    Logger *Log::logger = NullLogger::instance();

    void Log::set(Logger *logger) {
        Log::logger = logger;
    }

}