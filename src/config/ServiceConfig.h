// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "yaml-cpp/yaml.h"

#include "ServiceRestartMode.h"

namespace Atone {

    class ServiceConfig {
        public:
            ServiceConfig(std::string name);
            ~ServiceConfig();

            std::string name;
            size_t argc = 0;
            char **argv = NULL;
            std::vector<std::string> depends_on;
            ServiceRestartMode restart = ServiceRestartMode::No;

            static ServiceConfig *FromConfig(std::string name, YAML::Node config);

            void SetCommandArgs(YAML::Node command);
            void SetCommandArgs(std::string command);
            void SetCommandArgs(size_t argc, char **argv);
            void FreeCommandArgs();

            void SetDependsOn(YAML::Node depends_on);
            void SetRestartMode(YAML::Node restart);
    };

}