// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <string.h>

#include "ServiceConfig.h"
#include "ServiceStatus.h"

namespace Atone {

    class Service {
        private:
            struct ServiceState {
                ServiceStatus status = ServiceStatus::NotStarted;
                pid_t pid = 0;
                int exit_code;
            };

            std::shared_ptr<ServiceState> state;
            std::shared_ptr<ServiceConfig> config;

        public:
            Service();
            Service(ServiceConfig *config);

            static Service FromConfig(std::string name, YAML::Node config);

            std::string name();
            size_t argc();
            char **argv();
            std::vector<std::string> dependsOn();
            ServiceRestartMode restartMode();

            ServiceStatus status();
            bool isRunning();
            pid_t pid();
            int exitCode();
            bool canRestart();

            void Start();
            void Stop();
            bool CheckProcessState();
    };

}