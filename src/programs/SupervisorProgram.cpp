// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "SupervisorProgram.h"

#include "exception/AtoneException.h"
#include "config/AtoneOptions.h"
#include "config/Context.h"
#include "config/Supervisor.h"
#include "logging/Log.h"
#include "utils/constants.h"
#include "utils/time.h"

namespace Atone {
    SupervisorProgram::SupervisorProgram(const AtoneOptions &options)
        : ProgramBase(options) {
    }

    int SupervisorProgram::Run() {
        auto atone_pid = getpid();

        Log::info("starting... (PID=%i)", atone_pid);

        // start

        if (atone_pid != 1) {
            throw AtoneException("atone must run as init process (pid must be 1)");
        }

        auto atone = Context::FromOptions(options);
        auto services = atone.services;

        if (!atone.workdir.empty()) {
            std::filesystem::current_path(atone.workdir);
        }

        services.Start();

        // running

        bool terminate = false;
        while (!terminate) { // wait signal loop
            siginfo_t siginfo;
            int signum = Supervisor::WaitSignal(&siginfo);

            switch (signum) {
                case SIGHUP:
                case SIGINT:
                case SIGQUIT:
                case SIGTERM: {
                    Log::debug("signal received: %s", strsignal(signum));
                    terminate = true;
                    break;
                }
                case SIGCHLD: {
                    Log::debug("signal received: %s (PID=%i)", strsignal(signum), siginfo.si_pid);

                    if (ReapProcesses(services, true)) {
                        terminate = true;
                    }
                    
                    break;
                }
                default:
                    Log::debug("unhandled signal received: %s", strsignal(signum));
                    break;
            }
        }

        // stop

        timespec timeout = { 5, 0 };
        timeout_from_now(timeout);

        // stop all services before to terminate all processes
        // to allow the services gracefully terminates
        // their children processes 
        StopAllServices(services, timeout);
        KillAllProcess(services, timeout);
        
        Supervisor::ReapZombieProcess();

        Log::info("exit (PID=%i)", getpid());
        return 0;
    }

    /**
     * Request to reap all zombie processes.
     * 
     * @param services The services manager to collect the services.
     * @param restart_services If true, restart all services that can be restarted.
     * @return After all zombie processes is reaped, returns true if ll child processes is terminated.
     *         And false, if any child process is still running.
     */
    bool SupervisorProgram::ReapProcesses(ServicesManager &services, bool restart_services) {
        Log::debug("reaping child processes");

        while (true) {
            auto pid = Supervisor::CheckForExitedProcess();

            switch (pid) {
                case -1:
                    Log::debug("no more child process");
                    return true;
                case 0:
                    // no process exits, but one or more child processes still running
                    Log::debug("reaping child processes completed");
                    return false;
            }

            Log::debug("child process exits (PID=%i)", pid);

            Service *service;
            if (services.TryGetService(pid, service)) {
                // service process
                if (service->CheckProcessState())
                    throw AtoneException("invalid service process state");

                if (restart_services)
                    services.CheckService(*service);
            }
            else {
                // any other process
                Supervisor::ReapZombieProcess(pid);
            }
        }
    }

    void SupervisorProgram::KillAllProcess(ServicesManager &services, timespec timeout) {
        Log::trace("terminating child process");

        // sends TERM signal to all process
        if (kill(-1, SIGTERM) != 0) {
            auto err = errno;

            // completed
            if (err == ESRCH) {
                Log::notice("no child process is running");
                return;
            }

            throw std::system_error(err, std::system_category(), "terminating child process failed");
        }

        while (true) {

            siginfo_t siginfo;
            auto signum = Supervisor::WaitSignal(&siginfo, timeout);

            switch (signum) {

                case 0: { // timeout
                    Log::trace("killing child process");

                    // sends KILL signal to all process
                    if (kill(-1, SIGKILL) != 0) {
                        auto err = errno;

                        // completed
                        if (err == ESRCH) {
                            Log::notice("no child process is running");
                            return;
                        }

                        throw std::system_error(err, std::system_category(), "killing child process failed");
                    }

                    if (ReapProcesses(services, false))
                        return;

                    timeout = { 0, 1000000 /* 1ms */ };
                    timeout_from_now(timeout);

                    break;
                }

                case SIGCHLD: {
                    Log::debug("signal received: %s (PID=%i)", strsignal(signum), siginfo.si_pid);

                    if (ReapProcesses(services, false))
                        return;

                    break;
                }

                default: {
                    Log::debug("unhandled signal received: %s", strsignal(signum));
                    break;
                }

            }
        }

    }

    bool SupervisorProgram::StopAllServices(ServicesManager &services, timespec timeout) {
        Log::trace("stopping all services");

        if (services.Stop())
            return true;
        
        Log::trace("waiting one or more process to stop");

        while (services.isRunning()) {
            siginfo_t siginfo;
            auto signum = Supervisor::WaitSignal(&siginfo, timeout);

            switch (signum) {
                case 0: // timeout
                    Log::warn("stopping process timeout");
                    return false;

                case SIGCHLD: {
                    Log::debug("signal received: %s (PID=%i)", strsignal(signum), siginfo.si_pid);
                    ReapProcesses(services, false);
                    break;
                }
                default:
                    Log::debug("unhandled signal received: %s", strsignal(signum));
                    break;
            }
        }

        return true;
    }
}
