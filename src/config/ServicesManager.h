// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <map>

#include "AtoneOptions.h"
#include "Service.h"

namespace Atone {

    class ServicesManager {
        private:
            std::map<std::string, Service> services = std::map<std::string, Service>();

        public:
            ServicesManager();
            ServicesManager(Service service);
            ServicesManager(std::map<std::string, Service> services);

            bool isRunning();

            bool TryGetService(pid_t pid, Service *result = NULL);
            bool TryGetService(const std::string name, Service *result = NULL);

            void Start();
            bool Stop();
            bool CheckAllServices();
            bool CheckService(pid_t pid);
            bool CheckService(Service service);
    };

}