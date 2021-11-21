// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "ServicesManager.h"

#include <map>

#include "Service.h"
#include "exception/AtoneException.h"
#include "logging/Log.h"

namespace Atone {

    ServicesManager::ServicesManager() {}

    ServicesManager::ServicesManager(Service service) {
        this->services.insert(std::pair(service.name(), service));
    }

    ServicesManager::ServicesManager(YAML::Node document) {
        const auto services_node = document["services"];

        if (!services_node) {
            throw std::domain_error("services not found");
        }

        if (services_node.Type() != YAML::NodeType::Map) {
            throw std::domain_error("invalid services section value");
        }

        for (auto entry : services_node) {
            auto service_name = entry.first.as<std::string>();
            auto service = Service::FromConfig(service_name, entry.second);

            this->services.insert(std::pair(service_name, service));
        }
    }

    bool ServicesManager::TryGetService(pid_t pid, Service *result) {
        for (auto entry : services) {
            auto service = entry.second;

            if (service.pid() == pid) {
                if (result)
                    *result = service;
                return true;
            }
        }

        return false;
    }

    bool ServicesManager::TryGetService(const std::string name, Service *result) {
        auto entry = services.find(name);

        if (entry != services.end()) {
            if (result)
                *result = entry->second;
            return true;
        }

        return false;
    }

    void ServicesManager::Start() {
        for (auto entry : services) {
            auto service = entry.second;

            // start dependency services
            auto dependency_names = service.dependsOn();
            for (auto dependency_name : dependency_names) {
                auto dependency_service = services.at(dependency_name);
                dependency_service.Start();
            }

            service.Start();
        }
    }

    void ServicesManager::Stop() {
        for (auto entry : services) {
            auto service = entry.second;
            service.Stop();
        }
    }

    bool ServicesManager::CheckAllServices() {
        auto result = false;

        Log::trace("checking all services");

        for (auto entry : services) {
            result |= CheckService(entry.second);
        }

        Log::debug("no service is running");

        return result;
    }

    bool ServicesManager::CheckService(pid_t pid) {
        for (auto entry : services) {
            auto service = entry.second;

            if (service.pid() != pid)
                continue;
    
            return CheckService(service);
        }

        return false;
    }

    bool ServicesManager::CheckService(Service service) {
        auto service_name = service.name().c_str();

        Log::trace("%s: checking service", service_name);

        if (service.isRunning()) {
            Log::debug("%s: service is running", service_name);
            return true;
        }

        if (service.canRestart()) {
            Log::debug("%s: service must be restarted", service_name);

            service.Start();
            return true;
        }

        Log::debug("%s: service must not be restarted", service_name);

        return false;
    }

}