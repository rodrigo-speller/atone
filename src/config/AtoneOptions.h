// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

#include "AtoneMode.h"
#include "logging/LogLevel.h"
#include "logging/Logger.h"
#include "utils/constants.h"

namespace Atone {

    class AtoneOptions {
        public:
            typedef std::shared_ptr<Logger> (*LoggerFactory)(const AtoneOptions &options);

            // command line arguments input

            size_t argc = 0;
            char **argv = NULL;

            // atone mode

            AtoneMode mode = AtoneMode::Undefined;

            // multi-services

            std::string configFile = ATONE_OPTION_DEFAULT_CONFIGFILE;

            // single-service

            std::string serviceName = ATONE_OPTION_DEFAULT_SERVICENAME;
            size_t commandArgc = 0;
            char **commandArgv = NULL;

            // logging

            LoggerFactory loggerFactory = DefaultLoggerFactory;
            LogLevel logLevel = LogLevel::Trace;

            static AtoneOptions FromArgs(size_t argc, char **argv);
        private:
            static std::shared_ptr<Logger> DefaultLoggerFactory(const AtoneOptions &options);

            static LogLevel ParseLogLevel(const char *level);
            static LoggerFactory ParseLogger(const char *logger);
    };

}