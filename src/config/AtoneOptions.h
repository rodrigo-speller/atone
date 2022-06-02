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

            AtoneMode mode = AtoneMode::Undefined;
            bool usage = false;
            bool version = false;

            std::string errorMessage;
            std::string configFile = ATONE_OPTION_DEFAULT_CONFIGFILE;
            std::string serviceName = ATONE_OPTION_DEFAULT_SERVICENAME;
            size_t argc = 0;
            char **argv = NULL;
            size_t commandArgc = 0;
            char **commandArgv = NULL;
            LoggerFactory loggerFactory = DefaultLoggerFactory;
            LogLevel logLevel = LogLevel::Trace;

            void LoadArgs(size_t argc, char **argv);

        private:
            struct Option {
                struct optparse_long parser;
                AtoneMode mode;
            };

            LogLevel ParseLogLevel(const char *level);
            LoggerFactory ParseLogger(const char *logger);

            static std::shared_ptr<Logger> DefaultLoggerFactory(const AtoneOptions &options);
    };

}