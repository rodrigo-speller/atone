// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "Service.h"

#include <assert.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <system_error>

#include "Supervisor.h"
#include "ServiceConfig.h"
#include "ServiceStatus.h"
#include "exception/AtoneException.h"
#include "logging/Log.h"

namespace Atone {

    Service::Service(const ServiceConfig &config)
        : config(config) {
    }

    std::string Service::name() const { return config.name; }
    size_t Service::argc() const { return config.argc; }
    char **Service::argv() const { return config.argv; }
    std::vector<std::string> Service::dependsOn() const { return config.depends_on; }
    ServiceRestartMode Service::Service::restartMode() const { return config.restart; }

    ServiceStatus Service::status() const { return state.status; }
    bool Service::isRunning() const { return state.pid != 0; }
    pid_t Service::pid() const { return state.pid; }
    int Service::exitCode() const { return state.exit_code; }

    bool Service::canRestart() const {
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

    void Service::Start() {
        auto service_name = config.name.c_str();

        Log::info("%s: starting service", service_name);

        if (CheckProcessState()) {
            Log::info("%s: service already running", service_name);
            return;
        }

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

        state.pid = pid;
        state.status = ServiceStatus::Running;

        Log::info("%s: service started (PID=%i)", service_name, pid);
    }

    bool Service::Stop(bool _kill) {
        auto service_name = config.name.c_str();

        Log::info("%s: stopping service", service_name);

        if (!CheckProcessState()) {
            return true;
        }

        auto pid = state.pid;
        auto signal = _kill ? SIGKILL : SIGTERM;

        Log::trace("%s: sending signal to service process: %s", service_name, strsignal(signal));

        if (kill(pid, signal) != 0) {
            auto _errno = errno;

            if (_errno != ESRCH) {
                Log::crit("%s: service process kill failed: %s", service_name, strerror(_errno));
                throw std::system_error(_errno, std::system_category(), "kill failed");
            }
        }

        if (_kill) {
            waitid(P_PID, pid, nullptr, WNOWAIT | WEXITED);
        }

        return !CheckProcessState();
    }

    bool Service::CheckProcessState() {
        auto service_name = config.name.c_str();

        Log::trace("%s: checking service process", service_name);

        auto pid = state.pid;

        if (pid == 0) {
            Log::debug("%s: no process is assigned to the service", service_name);
            return false;
        }

        int wstatus = 0;
        auto result = waitpid(pid, &wstatus, WNOHANG);

        // on error, -1 is returned
        if (result < 0) {
            auto _errno = errno;
            Log::crit("%s: service process wait failed: %s (PID=%i)", service_name, strerror(_errno), pid);
            throw std::system_error(_errno, std::system_category(), "wait failed");
        }
        // one or more child(ren) specified by pid exist, but have not yet changed state, then 0 is returned
        else if (result == 0) {
            Log::debug("%s: service process is running (PID=%i)", service_name, pid);
            return true;
        }
        // on success, returns the process ID of the child whose state has changed
        assert(result == pid);

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