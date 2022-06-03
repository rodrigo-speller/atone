// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "ServicesManager.h"

#include "Service.h"
#include "exception/AtoneException.h"
#include "logging/Log.h"

namespace Atone {

    ServicesManager::ServicesManager() {}

    bool ServicesManager::isRunning() const {
        for (auto entry : services) {
            auto service = entry.second;

            if (service.isRunning())
                return true;
        }

        return false;
    }

    void ServicesManager::Add(const ServiceConfig &config) {
        auto service = new Service(config);
        services.insert({ service->name(), *service });
    }

    bool ServicesManager::TryGetService(const pid_t pid, Service *&result) const {
        for (auto entry : services) {
            auto service = entry.second;

            if (service.pid() == pid) {
                if (result)
                    result = &service;
                return true;
            }
        }

        return false;
    }

    bool ServicesManager::TryGetService(const std::string &name, Service *&result) const {
        auto entry = services.find(name);

        if (entry != services.end()) {
            if (result)
                result = &entry->second;
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

    /**
     * Request to stop all services.
     * @return Returns true if all services are stopped.
     *         Otherwise, if any service is still running, returns false.
     */
    bool ServicesManager::Stop() {
        auto success = true;

        // TODO: dependency order

        for (auto entry : services) {
            auto service = entry.second;
            success &= service.Stop();
        }

        return success;
    }

    bool ServicesManager::CheckAllServices() const {
        auto result = false;

        Log::trace("checking all services");

        for (auto entry : services) {
            result |= CheckService(entry.second);
        }

        Log::debug("no service is running");

        return result;
    }

    bool ServicesManager::CheckService(const pid_t pid) const {
        for (auto entry : services) {
            auto service = entry.second;

            if (service.pid() != pid)
                continue;
    
            return CheckService(service);
        }

        return false;
    }

    bool ServicesManager::CheckService(Service &service) const {
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