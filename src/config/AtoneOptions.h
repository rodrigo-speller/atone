// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "optparse.h"
#include <string>

#include "AtoneMode.h"
#include "logging/LogLevel.h"
#include "logging/Logger.h"
#include "utils/constants.h"

namespace Atone {

    class AtoneOptions {
        public:
            typedef Logger *(*LoggerFactory)(AtoneOptions options);

            AtoneMode mode = AtoneMode::Undefined;
            bool usage = false;

            std::string errorMessage;
            std::string configFile = ATONE_OPTION_DEFAULT_CONFIGFILE;
            std::string serviceName = ATONE_OPTION_DEFAULT_SERVICENAME;
            size_t argc = 0;
            char **argv = NULL;
            LoggerFactory loggerFactory = 0;
            LogLevel logLevel = LogLevel::Trace;

            void LoadArgs(size_t argc, char **argv);

            static void PrintUsage(char *program);

        private:
            struct Option {
                struct optparse_long parser;
                AtoneMode mode;
            };

            LogLevel ParseLogLevel(const char *level);
            LoggerFactory ParseLogger(const char *logger);
    };

}