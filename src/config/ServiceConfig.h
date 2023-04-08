// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

#include "service/ServiceRestartPolicy.h"

namespace Atone {

    class ServiceConfig {
        public:
            ServiceConfig(const std::string &name);

            std::string name;
            size_t argc = 0;
            shared_ptr<char *> argv = NULL;
            std::vector<std::string> depends_on;
            ServiceRestartPolicy restart = ServiceRestartPolicy::No;

            void SetCommandArgs(const std::string &command);
            void SetCommandArgs(const size_t argc, char **argv);

        private:
            void FreeCommandArgs();
    };

}