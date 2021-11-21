// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "NullLogger.h"
#include "LogLevel.h"
#include "Logger.h"

namespace Atone {

    NullLogger NullLogger::_instance = NullLogger();

    Logger *NullLogger::instance() { return &_instance; }

    void NullLogger::Log(LogLevel level, const char *format, ...) {}

}