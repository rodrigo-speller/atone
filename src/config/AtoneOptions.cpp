// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "AtoneOptions.h"

#include "optparse.h"
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "AtoneMode.h"
#include "logging/Logger.h"
#include "logging/NullLogger.h"
#include "logging/OutputLogger.h"
#include "logging/SyslogLogger.h"
#include "logging/TerminalLogger.h"
#include "utils/constants.h"

namespace Atone {

    void AtoneOptions::LoadArgs(size_t argc, char **argv) {
        struct optparse state;

        optparse_init(&state, argv);
        state.permute = 0;

        struct Option options[] = {
            {"config", 'c', OPTPARSE_REQUIRED, AtoneMode::MultiServices},
            {"help",   'h', OPTPARSE_NONE,     AtoneMode::Help         },
            {"log",    'l', OPTPARSE_REQUIRED, AtoneMode::Undefined    },
            {"logger", 'L', OPTPARSE_REQUIRED, AtoneMode::Undefined    },
            {"name",   'n', OPTPARSE_REQUIRED, AtoneMode::SingleService},
            {"version",'v', OPTPARSE_NONE,     AtoneMode::Version      },
            NULL
        };
        const int options_len = sizeof(options) / sizeof(Option);

        struct optparse_long options_long[options_len];
        for(int i = 0; i < options_len; i++) {
            auto option = options[i];
            options_long[i] = { option.parser.longname, option.parser.shortname, option.parser.argtype };
        }

        int option_index;
        while (optparse_long(&state, options_long, &option_index) != -1) {
            auto option = options[option_index];

            switch (option.parser.shortname) {
                case 'c':
                    configFile = state.optarg;
                    break;
                case 'h':
                    usage = true;
                    break;
                case 'l':
                    logLevel = ParseLogLevel(state.optarg);
                    break;
                case 'L':
                    loggerFactory = ParseLogger(state.optarg);
                    break;
                case 'n':
                    serviceName = state.optarg;
                    break;
                case 'v':
                    version = true;
                    break;
                case '?':
                    errorMessage = state.errmsg;
                    break;
            }

            if (option.mode != AtoneMode::Undefined) { // sets mode
                if (mode == AtoneMode::Undefined)
                    mode = option.mode;
                else if (mode != option.mode)
                    errorMessage = "invalid arguments";
            }
        }

        if (errorMessage.empty()) { // no error post execution
            if (argv[state.optind]) {
                if (mode == AtoneMode::Undefined) {
                    mode = AtoneMode::SingleService;
                }
                
                if (mode == AtoneMode::SingleService) {
                    this->argc = argc - state.optind;
                    this->argv = &argv[state.optind];
                }
                else {
                    errorMessage = "invalid arguments";
                }
            }
        }
    }

    void AtoneOptions::PrintUsage(char *program) {
        program = basename(program);

        std::cout << 
            "Atone, version " << ATONE_BUILD_VERSION_STAMP << "\n"
            "\n"
            "Starts a supervisor process that defines and runs services.\n"
            "\n"
            "Usage:\n"
            "  " << program << " [-c <file>] [options] \n"
            "  " << program << " [-n <name>] [options] [--] <cmd> <args...>\n"
            "  " << program << " -h|--help\n"
            "\n"
            "Multi-services mode options:\n"
            "  -c, --config FILE    Specifies the configuration file.\n"
            "                       (default: " << ATONE_OPTION_DEFAULT_CONFIGFILE << ")\n"
            "\n"
            "Single-service mode options:\n"
            "  -n, --name           Specifies the service name.\n"
            "                       (default: " << ATONE_OPTION_DEFAULT_SERVICENAME << ")\n"
            "\n"
            "General options:\n"
            "  -l, --log LEVEL      Defines the minimum log level.\n"
            "                       (default: Trace)\n"
            "                       The value must be one of:\n"
            "                           Emergency, Fatal, Critical, Error, Warning, Notice, Information, Debug or Trace.\n"
            "  -L, --logger LOGGER  Defines the logger type.\n"
            "                       (default: \"Terminal\" for tty attached or \"Output\")\n"
            "                       The value must be one of:\n"
            "                           Output:     Logs to the output.\n"
            "                           Terminal:   Logs to the output.\n"
            "                           Syslog:     Logs to system log.\n"
            "                           Null:       Don't logs.\n"
            "  -h, --help           Prints this help.\n"
            "  -v, --version        Prints the version.\n"
            "\n"
            "Atone homepage: <https://github.com/rodrigo-speller/atone>\n"
        ;
    }

    void AtoneOptions::PrintVersion() {
        std::cout << ATONE_BUILD_VERSION_STAMP << "\n";
    }

    LogLevel AtoneOptions::ParseLogLevel(const char *level) {
        const int sz = sizeof("information") + 1;
        char value[sz];

        // copy and normalize level to value
        int i;
        for (i = 0; i < sz; i++) {
            const auto c = value[i] = tolower(level[i]);
            if (c == 0)
                break;
        }

        if (i < sz) {

            // parse value
            switch (value[0])
            {
                case 'c':
                    if (strcmp(value, "critical") == 0)
                        return LogLevel::Critical;
                    break;
                case 'd':
                    if (strcmp(value, "debug") == 0)
                        return LogLevel::Debug;
                    break;
                case 'e':
                    switch (value[1])
                    {
                        case 'm':
                            if (strcmp(value, "emergency") == 0)
                                return LogLevel::Emergency;
                            break;
                        case 'r':
                            if (strcmp(value, "error") == 0)
                                return LogLevel::Error;
                            break;
                    }
                    break;
                case 'f':
                    if (strcmp(value, "fatal") == 0)
                        return LogLevel::Fatal;
                    break;
                case 'i':
                    if (strcmp(value, "information") == 0)
                        return LogLevel::Information;
                    break;
                case 'n':
                    if (strcmp(value, "notice") == 0)
                        return LogLevel::Notice;
                    break;
                case 't':
                    if (strcmp(value, "trace") == 0)
                        return LogLevel::Trace;
                    break;
                case 'w':
                    if (strcmp(value, "warning") == 0)
                        return LogLevel::Warning;
                    break;
            }

        }

        throw std::domain_error("invalid log level");
    }

    AtoneOptions::LoggerFactory AtoneOptions::ParseLogger(const char *logger) {
        const int sz = sizeof("terminal") + 1;
        char value[sz];

        // copy and normalize logger to _buffer
        int i;
        for (i = 0; i < sz; i++) {
            const auto c = value[i] = tolower(logger[i]);
            if (c == 0)
                break;
        }

        if (i < sz) {

            // parse
            switch (value[0])
            {
                case 'n':
                    if (strcmp(value, "null") == 0)
                        return [](AtoneOptions options) {
                            return NullLogger::instance();
                        };
                    break;
                case 'o':
                    if (strcmp(value, "output") == 0)
                        return [](AtoneOptions options) {
                            return (Logger *)new OutputLogger(options.logLevel);
                        };
                    break;
                case 's':
                    switch (value[1])
                    {
                        case 'i':
                            if (strcmp(value, "simple") == 0)
                                return [](AtoneOptions options) {
                                    return (Logger *)new NullLogger();
                            };
                            break;
                        case 'y':
                            if (strcmp(value, "syslog") == 0)
                                return [](AtoneOptions options) {
                                    return (Logger *)new SyslogLogger();
                            };
                            break;
                    }
                    break;
                case 't':
                    if (strcmp(value, "terminal") == 0)
                        return [](AtoneOptions options) {
                            return (Logger *)new TerminalLogger(options.logLevel);
                        };
                    break;
            }

        }

        throw std::domain_error("invalid logger");
    }

}