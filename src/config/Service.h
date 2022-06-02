// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

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

            ServiceConfig config;
            ServiceState state;

        public:
            Service(const ServiceConfig &config);

            std::string name() const;
            size_t argc() const;
            char **argv() const;
            std::vector<std::string> dependsOn() const;
            ServiceRestartMode restartMode() const;

            ServiceStatus status() const;
            bool isRunning() const;
            pid_t pid() const;
            int exitCode() const;
            bool canRestart() const;

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