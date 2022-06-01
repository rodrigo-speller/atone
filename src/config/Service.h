// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <memory>
#include <string>

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
            Service(std::shared_ptr<ServiceConfig> config);

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

            /**
             * Stops the service. If the service proceess is running, send a SIGTERM signal.
             * 
             * @return Returns true if the service is stopped or already is not running. Returns false if the service
             * process is still running, after the signal is sent.
             */
            bool Stop(bool kill = false);

            /**
             * Checks if the service process is running.
             * @return Returns true if the process is running.
             */
            bool CheckProcessState();
    };

}