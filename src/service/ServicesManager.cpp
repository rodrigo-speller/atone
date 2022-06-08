// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "ServicesManager.h"

#include "exception/AtoneException.h"
#include "logging/Log.h"
#include "service/Service.h"

namespace Atone {
    /**
     * Checks if any service is running.
     * @return Returns true if any service is running, otherwise false.
     */
    bool ServicesManager::isRunning() const {
        for (auto entry : services) {
            auto service = entry.second;

            if (service->isRunning())
                return true;
        }

        return false;
    }

    /**
     * Add a service to the manager.
     * @param config The service configuration.
     */
    void ServicesManager::AddService(const ServiceConfig &config) {
        auto service = new Service(this, config);
        services.insert({ service->name(), service });
    }

    /**
     * Get a service by name.
     * @param name The service name.
     * @return Returns the service.
     */
    Service *ServicesManager::GetService(const string &name) const {
      Service *result = nullptr;
      if (!TryGetService(name, result)) {
        throw AtoneException("service not found: " + name);
      }

      return result;
    }

    /**
     * Try to get a service by its PID.
     * @param pid The PID of the service.
     * @param result The service.
     * @return Returns true if the service was found, otherwise false.
     */
    bool ServicesManager::TryGetService(const pid_t pid, Service *&result) const {      
        for (auto entry : services) {
            auto service = entry.second;

            if (service->pid() == pid) {
                result = service;
                return true;
            }
        }
        return false;
    }

    /**
     * Tries to get a service by name.
     * @param name The service name.
     * @param result The service result.
     * @return Returns true if the service was found, otherwise false.
     */
    bool ServicesManager::TryGetService(const std::string &name, Service *&result) const {
        auto entry = services.find(name);

        if (entry != services.end()) {
            result = entry->second;
            return true;
        }
        return false;
    }

    /**
     * Request to start all services.
     */
    void ServicesManager::Start() {
        for (auto entry : services) {
            auto service = entry.second;
            service->Start();
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
            success &= service->Stop();
        }

        return success;
    }

    /**
     * Request to check the state of all services. If any service is not running,
     * and it can be restarted, it will be restarted.
     * @return Returns true if all services are running.
     *         Otherwise, if any service is not running, returns false.
     */
    bool ServicesManager::CheckAllServices() const {
        auto result = false;

        for (auto entry : services) {
            result |= CheckService(entry.second);
        }

        return result;
    }

    /**
     * Request to check the state of a service. If the service is not running,
     * and it can be restarted, it will be restarted.
     * @param service The service to check.
     * @return Returns true if the service is running.
     *         Otherwise, if the service is not running, returns false.
     */
    bool ServicesManager::CheckService(Service *service) const {
        auto service_name = service->name().c_str();

        Log::trace("%s: checking service", service_name);

        if (service->isRunning()) {
            Log::debug("%s: service is running", service_name);
            return true;
        }

        if (service->canRestart()) {
            Log::debug("%s: service must be restarted", service_name);

            service->Start();
            return true;
        }

        Log::debug("%s: service must not be restarted", service_name);

        return false;
    }
}