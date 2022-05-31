// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "SupervisorProgram.h"

#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

#include "exception/AtoneException.h"
#include "config/AtoneOptions.h"
#include "config/Context.h"
#include "config/Supervisor.h"
#include "logging/Log.h"
#include "utils/constants.h"
#include "utils/time.h"

namespace Atone {
    bool reap_processes(ServicesManager &services, bool restart = false);

    void kill_all_process(ServicesManager &services, timespec timeout);
    bool stop_all_services(ServicesManager &services, timespec timeout);

    int SupervisorProgram::Run(AtoneOptions &options) {
        auto atone_pid = getpid();

        Log::info("starting... (PID=%i)", atone_pid);

        // start

        if (atone_pid != 1) {
            throw AtoneException("atone must run as init process (pid must be 1)");
        }

        auto atone = Atone::Context(options);
        auto services = atone.services;

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

                    if (reap_processes(services, true)) {
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

        // stop all services before to kill all process
        // to allow the services gracefully terminates
        // their children processes 
        stop_all_services(services, timeout);
        kill_all_process(services, timeout);
        
        Supervisor::ReapZombieProcess();

        Log::info("exit (PID=%i)", getpid());
        return 0;
    }

    
    bool reap_processes(ServicesManager &services, bool restart_services) {
        siginfo_t siginfo;

        while (true) {
            // WNOHANG: zero out the si_pid field before the call and check for a nonzero value in this field after the call
            siginfo.si_pid = 0;
            if (waitid(P_ALL, 0, &siginfo, WNOWAIT | WNOHANG | WEXITED) != 0) {
                auto _errno = errno;

                if (_errno == ECHILD) {
                    Log::notice("no child process is running");
                    return true;
                }

                throw std::system_error(_errno, std::system_category(), "wait failed");
            }

            auto pid = siginfo.si_pid;
            // WNOHANG: zero out the si_pid field before the call and check for a nonzero value in this field after the call
            if (pid == 0)
                return false;

            Log::debug("child process exits (PID=%i)", pid);

            Service service;
            if (services.TryGetService(pid, &service)) {
                if (service.CheckProcessState())
                    throw AtoneException("invalid service process state");

                if (restart_services)
                    services.CheckService(service);
            }
            else {
                Supervisor::ReapZombieProcess(pid);
            }
        }
    }

    void kill_all_process(ServicesManager &services, timespec timeout) {
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

                    if (reap_processes(services, false))
                        return;

                    timeout = { 0, 1000000 /* 1ms */ };
                    timeout_from_now(timeout);

                    break;
                }

                case SIGCHLD: {
                    Log::debug("signal received: %s (PID=%i)", strsignal(signum), siginfo.si_pid);

                    if (reap_processes(services, false))
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

    bool stop_all_services(ServicesManager &services, timespec timeout) {
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
                    reap_processes(services, false);
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
