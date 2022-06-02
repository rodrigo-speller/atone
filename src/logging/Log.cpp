// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "Log.h"

#include "NullLogger.h"
#include "LogLevel.h"
#include "Logger.h"

namespace Atone {

    std::shared_ptr<Logger> Log::logger = std::make_shared<NullLogger>();

    void Log::set(const std::shared_ptr<Logger> &logger) {
        if (logger == nullptr) {
            auto nullLogger = std::make_shared<NullLogger>();
            Log::trace("logging stoped");
            Log::logger = nullLogger;
        } else {
            Log::trace("logging stoped");
            Log::logger = logger;
            Log::trace("logging started");
        }
    }

}