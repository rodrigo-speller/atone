// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

#include "AtoneOptions.h"
#include "Service.h"

namespace Atone {

    class ServicesManager {
        private:
            std::unordered_map<std::string, Service&> services;

        public:
            bool isRunning() const;

            void Add(const ServiceConfig &config);
            bool TryGetService(const pid_t pid, Service *&result) const;
            bool TryGetService(const std::string &name, Service *&result) const;

            void Start();
            bool Stop();
            bool CheckAllServices() const;
            bool CheckService(Service &service) const;
    };

}