// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "Log.h"

#include <cstdarg>
#include <memory>
#include <syslog.h>

#include "NullLogger.h"
#include "LogLevel.h"
#include "Logger.h"

namespace Atone {

    std::shared_ptr<Logger> Log::logger = std::make_shared<NullLogger>();

    void Log::set(std::shared_ptr<Logger> logger) {
        if (logger == nullptr) {
            Log::logger = std::make_shared<NullLogger>();
        } else {
            Log::logger = logger;
        }
    }

}