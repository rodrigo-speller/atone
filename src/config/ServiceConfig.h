// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

#include "service/ServiceRestartMode.h"

namespace Atone {

    class ServiceConfig {
        public:
            ServiceConfig(const std::string &name);
            ~ServiceConfig();

            std::string name;
            size_t argc = 0;
            char **argv = NULL;
            std::vector<std::string> depends_on;
            ServiceRestartMode restart = ServiceRestartMode::No;

            void SetCommandArgs(const std::string &command);
            void SetCommandArgs(const size_t argc, char **argv);

        private:
            void FreeCommandArgs();
    };

}