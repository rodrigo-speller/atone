// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "AtoneOptions.h"

#include "config/AtoneMode.h"
#include "logging/Logger.h"
#include "logging/NullLogger.h"
#include "logging/OutputLogger.h"
#include "logging/SyslogLogger.h"
#include "logging/TerminalLogger.h"

namespace Atone {
    /**
     * Loads the options from the command line arguments.
     * @param argc The number of arguments.
     * @param argv The arguments.
     * @returns The options.
     */
    AtoneOptions AtoneOptions::FromArgs(size_t argc, char **argv) {
        // INITIALIZE OPTIONS DEFINTION

        struct OptionDef {
            const struct optparse_long optparse;
            const AtoneMode mode;
        };

        // the options definition matrix
        const struct OptionDef options_defs[] = {
            {"config", 'c', OPTPARSE_OPTIONAL, AtoneMode::MultiServices},
            {"help",   'h', OPTPARSE_NONE,     AtoneMode::Help         },
            {"log",    'l', OPTPARSE_REQUIRED, AtoneMode::Undefined    },
            {"logger", 'L', OPTPARSE_REQUIRED, AtoneMode::Undefined    },
            {"name",   'n', OPTPARSE_REQUIRED, AtoneMode::SingleService},
            {"version",'v', OPTPARSE_NONE,     AtoneMode::Version      },
            NULL
        };

        // initialize optparse options definition matrix
        
        const int options_len = sizeof(options_defs) / sizeof(OptionDef);

        struct optparse_long options_long[options_len];
        for(int i = 0; i < options_len; i++) {
            auto option = options_defs[i];
            options_long[i] = option.optparse;
        }

        // PARSE OPTIONS

        AtoneOptions options;

        options.argc = argc;
        options.argv = argv;

        int option_index;
        int option_char;
        struct optparse state;
        optparse_init(&state, argv);
        while ((option_char = optparse_long(&state, options_long, &option_index)) != -1) {
            switch (option_char) {
                case 'c': // config
                    if (state.optarg) {
                        options.configFile = state.optarg;
                    }
                    break;
                case 'h': // help
                    // sets the mode to AtoneMode::Help
                    break;
                case 'l': // log
                    options.logLevel = ParseLogLevel(state.optarg);
                    break;
                case 'L': // logger
                    options.loggerFactory = ParseLogger(state.optarg);
                    break;
                case 'n': // name
                    options.serviceName = state.optarg;
                    break;
                case 'v': // version
                    // sets the mode to AtoneMode::Version
                    break;
                case '?': // error
                    if (state.errmsg[0] == '\0') {
                        throw std::domain_error("unknown error");
                    }
                    throw std::domain_error(state.errmsg);
            }

            // sets the mode to AtoneMode::SingleService if the service name is defined
            const auto &optionDef = options_defs[option_index];

            // sets the mode of atone according to the option
            if (optionDef.mode != AtoneMode::Undefined) {
                if (options.mode == AtoneMode::Undefined) {
                    options.mode = optionDef.mode;
                } else if (options.mode != optionDef.mode) {
                    throw std::domain_error(
                        std::string("unexpected option -- '")
                        + optionDef.optparse.longname
                        + "'"
                    );
                }
            }
        }

        // CHECK FOR REMAINING ARGS

        if (argv[state.optind]) {
            // if has remaining args, then the default mode is single-service
            if (options.mode == AtoneMode::Undefined) {
                options.mode = AtoneMode::SingleService;
            }
            
            if (options.mode == AtoneMode::SingleService) {
                // remaining args are the command args for the service on single-service mode
                options.commandArgc = argc - state.optind;
                options.commandArgv = &argv[state.optind];
            }
            else {
                // if the mode is not single-service, then the remaining args
                // are invalid
                throw std::domain_error("invalid arguments");
            }
        }

        return options;
    }

    /**
     * Parses the log level from string.
     * @param logLevel The log level string.
     * @return The log level value.
     */
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

    /**
     * Parses the logger factory function from string.
     * @param logger The logger factory string.
     * @return The logger factory function.
     */
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
                        return [](const AtoneOptions &options) {
                            return std::shared_ptr<Logger>(new NullLogger());
                        };
                    break;
                case 'o':
                    if (strcmp(value, "output") == 0)
                        return [](const AtoneOptions &options) {
                            return std::shared_ptr<Logger>(new OutputLogger(options.logLevel));
                        };
                    break;
                case 's':
                    switch (value[1])
                    {
                        case 'i':
                            if (strcmp(value, "simple") == 0)
                                return [](const AtoneOptions &options) {
                                    return std::shared_ptr<Logger>(new NullLogger());
                            };
                            break;
                        case 'y':
                            if (strcmp(value, "syslog") == 0)
                                return [](const AtoneOptions &options) {
                                    return std::shared_ptr<Logger>(new SyslogLogger());
                            };
                            break;
                    }
                    break;
                case 't':
                    if (strcmp(value, "terminal") == 0)
                        return [](const AtoneOptions &options) {
                            return std::shared_ptr<Logger>(new TerminalLogger(options.logLevel));
                        };
                    break;
            }

        }

        throw std::domain_error("invalid logger");
    }

    /**
     * Builds the default logger. This method defines the default logger factory
     * for the options.
     * @param options The atone options to get the log level.
     * @return A shared pointer to the built logger.
     */
    std::shared_ptr<Logger> AtoneOptions::DefaultLoggerFactory(const AtoneOptions &options) {
        if (isatty(1)) {
            // if stdout is a terminal, then use the terminal logger to
            // print styled output
            return std::make_shared<TerminalLogger>(options.logLevel);
        }

        // otherwise, use the output logger to print to stdout
        return std::make_shared<OutputLogger>(options.logLevel);
    }
}