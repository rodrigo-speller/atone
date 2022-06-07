// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

namespace Atone {
  class ServicesManager;
}

#include "config/AtoneOptions.h"
#include "service/Service.h"

namespace Atone {

    class ServicesManager {
        private:
            std::unordered_map<std::string, Service&> services;

        public:
            bool isRunning() const;

            void AddService(const ServiceConfig &config);
            Service &GetService(const string &name) const;
            bool TryGetService(const pid_t pid, Service *&result) const;
            bool TryGetService(const std::string &name, Service *&result) const;

            void Start();
            bool Stop();
            bool CheckAllServices() const;
            bool CheckService(Service &service) const;
    };

}