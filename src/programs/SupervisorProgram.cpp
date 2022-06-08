// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "SupervisorProgram.h"

#include "exception/AtoneException.h"
#include "config/AtoneOptions.h"
#include "config/Context.h"
#include "logging/Log.h"
#include "system/Supervisor.h"
#include "utils/constants.h"
#include "utils/time.h"

namespace Atone {
    SupervisorProgram::SupervisorProgram(const AtoneOptions &options)
        : ProgramBase(options) {
    }

    int SupervisorProgram::Run() {
        auto pid = getpid();
        bool exitcode = EXIT_SUCCESS;

        // setup

        Log::set(options.loggerFactory(options));
        Supervisor::Initialize();

        Log::info("starting supervisor... (PID=%i)", pid);

        // atone supervisor must be executed as an init process (pid = 1)
        // to adopt orphan processes, to be able to terminate, kill and reap them
        if (pid != 1) {
            throw AtoneException("atone must run as init process (pid must be 1)");
        }

        // execution context
        auto context = Context::FromOptions(options);

        Bootstrap(context);
        MainLoop(context);
        if (!Shutdown(context)) {
            exitcode = EXIT_FAILURE;
        }

        Log::info("exit supervisor (PID=%i)", getpid());

        // exit

        Supervisor::Dispose();
        Log::set(nullptr);

        return exitcode;
    }

    /**
     * Supervisor bootstrap.
     * @param context Execution context.
     */
    void SupervisorProgram::Bootstrap(Context &context) {
        auto &services = context.services;

        // changes current working directory, if specified
        if (!context.workdir.empty()) {
            std::filesystem::current_path(context.workdir);
        }

        // starts the services
        services->Start();
    }

    /**
     * Supervisor main loop.
     * @param context Execution context.
     */
    void SupervisorProgram::MainLoop(Context &context) {
        auto &services = context.services;

        // wait signal loop
        while (true) {
            siginfo_t siginfo;
            int signum = Supervisor::WaitSignal(&siginfo);

            switch (signum) {
                // Hangup detected on controlling terminal or death of controlling process
                case SIGHUP:
                // interrupt by user
                case SIGINT:
                // terminate process with core dump
                case SIGQUIT:
                // terminate process
                case SIGTERM: {
                    Log::notice("signal received: %s", strsignal(signum));
                    return;
                }
                // child process exited
                case SIGCHLD: {
                    Log::debug("signal received: %s (PID=%i)", strsignal(signum), siginfo.si_pid);

                    // reap child process
                    if (ReapProcesses(services, true)) {
                        // no more processes still running
                        return;
                    }
                    
                    break;
                }
                // any other signal
                default:
                    Log::debug("unhandled signal received: %s", strsignal(signum));
                    break;
            }
        }
    }

    /**
     * Supervisor shutdown.
     * @param context Execution context.
     */
    bool SupervisorProgram::Shutdown(Context &context) {
        bool success = true;
        auto services = context.services;

        // TODO: define a timeout for stopping services and terminate all processes
        timespec timeout = { 5, 0 };
        timeout_from_now(timeout);

        // stop all services
        // to allow the services gracefully terminates their children processes
        StopAllServices(services, timeout);

        // terminate remaining processes
        if (!TerminateAllProcess(services, timeout)) {
            Log::crit("failed on terminate all processes");
            success = false;
        }

        // at this point, all processes are terminated (or killed)
        // them reap any remaining zombie processes
        Supervisor::ReapZombieProcess();
        return success;
    }

    /**
     * Request to reap all zombie processes.
     * 
     * @param services The services manager to collect the services.
     * @param restart_services If true, restart all services that can be restarted.
     * @return After all zombie processes is reaped, returns true if all child processes is terminated.
     *         And false, if any child process is still running.
     */
    bool SupervisorProgram::ReapProcesses(ServicesManager *services, bool restart_services) {
        Log::debug("reaping child processes");

        while (true) {
            auto pid = Supervisor::CheckForExitedProcess();

            switch (pid) {
                case -1:
                    Log::notice("no child process is running");
                    return true;
                case 0:
                    // no process exits, but one or more child processes still running
                    Log::debug("reaping child processes completed");
                    return false;
            }

            Log::debug("child process exits (PID=%i)", pid);

            Service *service;
            if (services->TryGetService(pid, service)) {
                // service process
                if (service->CheckProcessState())
                    throw AtoneException("invalid service process state");

                if (restart_services)
                    services->CheckService(service);
            }
            else {
                // any other process
                Supervisor::ReapZombieProcess(pid);
            }
        }
    }

    /**
     * Terminate (or kill) all child processes.
     * @param services The services manager to collect the services.
     * @param timeout The timeout to wait for all child processes to terminate.
     *                After the timeout, all remaining child processes will be killed.
     * @return Returns true if all child processes was gracefully terminated, otherwise false.
     */
    bool SupervisorProgram::TerminateAllProcess(ServicesManager *services, timespec timeout) {
        Log::trace("terminating child process");

        // sends TERM signal to all process
        if (!Supervisor::SendSignal(-1, SIGTERM)) {
            Log::notice("no child process is running");
            return true;
        }

        // graceful termination status
        // starts as true, because no KILL signal has been sent yet
        bool gracefully = true;
        while (true) {
            siginfo_t siginfo;
            auto signum = Supervisor::WaitSignal(&siginfo, timeout);

            switch (signum) {
                // wait signal has timed-out
                // sends KILL signal to kill all process
                // if any process is still running: loop again
                case 0: {
                    Log::warn("stopping process timeout");
                    Log::trace("killing child process");

                    // sends KILL signal to all process
                    if (!Supervisor::SendSignal(-1, SIGKILL)) {
                        return gracefully;
                    }

                    // after this point, any exit signal from the child process
                    // will be treated as a consequence of the KILL signal
                    gracefully = false;

                    // updates the timeout to wait for a short time in the next iteration
                    // TODO: defines short-timeout value
                    timeout = { 0, 1000000 /* 1ms */ };
                    timeout_from_now(timeout);

                    break;
                }
                // child process exits
                // collect the child processes and loop again
                case SIGCHLD: {
                    Log::debug("signal received: %s (PID=%i)", strsignal(signum), siginfo.si_pid);

                    // reap the child processes
                    if (ReapProcesses(services, false)) {
                        // no more child process still running
                        return gracefully;
                    }

                    break;
                }
                // any other signal
                // just loop again
                default: {
                    Log::debug("unhandled signal received: %s", strsignal(signum));
                    break;
                }
            }
        }
    }

    /**
     * Stop all services.
     * @param services The services manager to collect the services.
     * @param timeout The timeout to wait for all services to stop.
     */
    bool SupervisorProgram::StopAllServices(ServicesManager *services, timespec timeout) {
        Log::trace("stopping all services");

        // request to stop all services
        if (services->Stop()) {
            return true;
        }
        
        Log::trace("awaiting remaining services to stop");
        while (services->isRunning()) {
            siginfo_t siginfo;
            auto signum = Supervisor::WaitSignal(&siginfo, timeout);

            switch (signum) {
                // wait signal has timed-out
                // returns with false
                case 0:
                    Log::warn("service stop timed-out");
                    return false;

                // child process exits
                // collect the child processes and loop again
                case SIGCHLD: {
                    Log::debug("signal received: %s (PID=%i)", strsignal(signum), siginfo.si_pid);

                    // reap the child processes
                    if (ReapProcesses(services, false)) {
                        // no more child process still running
                        // returns with true
                        return true;
                    }
                    break;
                }
                // any other signal
                // just loop again
                default:
                    Log::debug("unhandled signal received: %s", strsignal(signum));
                    break;
            }
        }

        // no more service is running
        // returns with true
        return true;
    }
}
