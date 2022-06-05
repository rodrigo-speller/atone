// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "Service.h"

#include "Supervisor.h"
#include "ServiceConfig.h"
#include "ServiceStatus.h"
#include "exception/AtoneException.h"
#include "logging/Log.h"
#include "utils/time.h"

namespace Atone {

    Service::Service(const ServiceConfig &config)
        : config(config) {
    }

    std::string Service::name() const { return config.name; }
    size_t Service::argc() const { return config.argc; }
    char **Service::argv() const { return config.argv; }
    std::vector<std::string> Service::dependsOn() const { return config.depends_on; }
    ServiceRestartMode Service::restartMode() const { return config.restart; }

    ServiceStatus Service::status() const { return state.status; }
    bool Service::isRunning() const { return state.pid != 0; }
    pid_t Service::pid() const { return state.pid; }
    int Service::exitCode() const { return state.exit_code; }

    /**
     * Defines if the service can be restarted.
     */
    bool Service::canRestart() const {
        if (isRunning()) {
            return false;
        }

        switch (restartMode()) {
            case ServiceRestartMode::No:
                return false;
            case ServiceRestartMode::Always:
                return true;
            case ServiceRestartMode::OnFailure:
                return (exitCode() != EXIT_SUCCESS);
                break;
            case ServiceRestartMode::UnlessStopped:
                return (status() != ServiceStatus::Stopped);
                break;
            default:
                throw std::domain_error("invalid restart mode");
        }
    }

    /**
     * Starts the service. If the service is already running,
     * this method does nothing.
     */
    void Service::Start() {
        auto service_name = config.name.c_str();
        Log::info("%s: starting service", service_name);

        // check if the service and, eventually, its process are running
        if (CheckProcessState()) {
            Log::info("%s: service already running", service_name);
            return;
        }

        // spawn process

        pid_t pid;
        auto argv = this->argv();
        try {
            Log::debug("%s: spawning service", service_name);
            pid = Supervisor::Spawn(argv);
        }
        catch (...) {
            Log::error("%s: failed to spawn service", service_name);
            state.status = ServiceStatus::Broken;
            throw;
        }

        // update state
        state.pid = pid;
        state.status = ServiceStatus::Running;

        Log::info("%s: service started (PID=%i)", service_name, pid);
    }

    /**
     * Stops the service. If the service is not running,
     * this method does nothing.
     * 
     * @return Returns true if the service was stopped. Otherwise, false.
     */
    bool Service::Stop() {
        auto service_name = config.name.c_str();
        Log::info("%s: stopping service", service_name);

        // check if the service and, eventually, its process are running
        if (!CheckProcessState()) {
            return true;
        }

        // send the SIGTERM signal to the service process
        auto pid = state.pid;
        Log::trace("%s: sending signal to service process: %s", service_name, strsignal(SIGTERM));
        Supervisor::SendSignal(pid, SIGTERM);

        // await for the process to exit
        // TODO: define a timeout
        timespec timeout = { 5, 0 };
        timeout_from_now(timeout);
        Supervisor::CheckForExitedProcess(pid, timeout);

        // checks the process state to reap it and update the service
        return !CheckProcessState();
    }

    /**
     * Checks if the process of the service is running. If the process is a
     * zombie, it is reaped and the service status is updated.
     * 
     * @return Returns true if the process of the service is running,
     *         otherwise false.
     */
    bool Service::CheckProcessState() {
        auto service_name = config.name.c_str();
        Log::trace("%s: checking service process", service_name);

        // checks if the service has a pid
        auto pid = state.pid;
        if (pid == 0) {
            Log::debug("%s: no process is assigned to the service", service_name);
            return false;
        }

        // checks if the process is running

        int wstatus = 0;
        Supervisor::ReapZombieProcess(pid, &wstatus);

        if (wstatus == 0) {
            Log::debug("%s: service process is running (PID=%i)", service_name, pid);
            return true;
        }

        // updates the service status to broken
        state.pid = 0;
        state.status = ServiceStatus::Broken;

        if (WIFEXITED(wstatus)) {
            Log::info("%s: service process exits (PID=%i, EXITCODE=%i)", service_name, pid, WEXITSTATUS(wstatus));
        } else {
            Log::info("%s: service process terminated by signal (PID=%i, SIGNAL=%i)", service_name, pid, WTERMSIG(wstatus));
        }
        return false;
    }

}