// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "ServiceConfig.h"

#include "ServiceRestartMode.h"
#include "exception/AtoneException.h"

namespace Atone {

    ServiceConfig::ServiceConfig(const std::string &name) { this->name = name; }
    ServiceConfig::~ServiceConfig() { FreeCommandArgs(); }

    void ServiceConfig::SetCommandArgs(const std::string &command) {
        wordexp_t we_args;

        if (wordexp(command.c_str(), &we_args, 0) != 0) {
            throw AtoneException("invalid service command");
        }

        SetCommandArgs(we_args.we_wordc, we_args.we_wordv);
        wordfree(& we_args);
    }

    void ServiceConfig::SetCommandArgs(const size_t argc, char ** argv) {
        FreeCommandArgs();

        char **dst_argv = new char *[argc + 1];

        for (size_t i = 0; i < argc; i++) {
            const auto src_arg = argv[i];
            const auto len = strlen(src_arg);
            dst_argv[i] = strcpy(new char[len + 1], src_arg);
        }
        dst_argv[argc] = NULL;

        this->argv = dst_argv;
        this->argc = argc;
    }

    void ServiceConfig::FreeCommandArgs() {
        const auto argv = this->argv;
        if (argv != NULL) {
            const auto argc = this->argc;

            this->argv = NULL;
            this->argc = 0;

            for (size_t i = 0; i < argc; i++) {
                delete argv[i];
            }
            delete argv;
        }
    }
}