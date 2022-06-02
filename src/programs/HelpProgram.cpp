// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "HelpProgram.h"

#include <filesystem>
#include <iostream>
#include <libgen.h>

#include "config/AtoneOptions.h"
#include "utils/constants.h"

namespace Atone {
    HelpProgram::HelpProgram(const AtoneOptions &options)
        : ProgramBase(options) {
    }

    int HelpProgram::Run() {
        auto program = options.argv[0];
        program = basename(program);
        PrintUsage(program);

        return 0;
    }

    void HelpProgram::PrintUsage(char *program) {
        std::cout << 
            "Atone, version " << ATONE_BUILD_VERSION_STAMP << "\n"
            "\n"
            "Starts a supervisor process that defines and runs services.\n"
            "\n"
            "Usage:\n"
            "  " << program << " --config[=<file>] [options] \n"
            "  " << program << " [--name=<name>] [options] [--] <cmd> <args...>\n"
            "  " << program << " -h|--help\n"
            "  " << program << " -v|--version\n"
            "\n"
            "Multi-services mode options:\n"
            "  -c, --config=FILE    Specifies the configuration file.\n"
            "                       (default: " << ATONE_OPTION_DEFAULT_CONFIGFILE << ")\n"
            "\n"
            "Single-service mode options:\n"
            "  -n, --name=NAME      Specifies the service name.\n"
            "                       (default: " << ATONE_OPTION_DEFAULT_SERVICENAME << ")\n"
            "\n"
            "General options:\n"
            "  -l, --log=LEVEL      Defines the minimum log level.\n"
            "                       (default: trace)\n"
            "                       The value must be one of:\n"
            "                           emergency, fatal, critical, error, warning, notice, information, debug or trace.\n"
            "  -L, --logger=LOGGER  Defines the logger type.\n"
            "                       (default: \"terminal\" for tty attached or \"output\")\n"
            "                       The value must be one of:\n"
            "                           output:     Logs to the output.\n"
            "                           terminal:   Logs to the output.\n"
            "                           syslog:     Logs to system log.\n"
            "                           null:       Don't logs.\n"
            "  -h, --help           Prints this help.\n"
            "  -v, --version        Prints the version.\n"
            "\n"
            "Atone homepage: <https://github.com/rodrigo-speller/atone>\n"
        ;
    }
}