// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#define OPTPARSE_IMPLEMENTATION

#include <filesystem>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

#include "config/AtoneOptions.h"
#include "config/Context.h"
#include "config/ServicesManager.h"
#include "config/Supervisor.h"
#include "exception/AtoneException.h"
#include "logging/Log.h"
#include "logging/OutputLogger.h"
#include "logging/TerminalLogger.h"

using namespace Atone;

int atone_main(int argc, char **argv) {
    Log::info("starting... (PID=%i)", getpid());

    // parse args

    AtoneOptions options;
    options.LoadArgs(argc, argv);

    if (!options.errorMessage.empty()) { // options error
        Log::fatal("load options failed: %s", options.errorMessage.c_str());
        return 1;
    }

    if (options.usage) { // usage
        AtoneOptions::PrintUsage(argv[0]);
        return 0;
    }

    // setup

    Logger *logger;

    if (options.loggerFactory) {
        logger = options.loggerFactory(options);
    } else {
        // default logger
        if (isatty(1)) {
            logger = new TerminalLogger(options.logLevel);
        } else {
            logger = new OutputLogger(options.logLevel);
        }
    }

    Log::set(logger);

    // start

    auto atone = Atone::Context(options);
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

                while (true) {
                    // WNOHANG: zero out the si_pid field before the call and check for a nonzero value in this field after the call
                    siginfo.si_pid = 0;
                    if (waitid(P_ALL, 0, &siginfo, WNOWAIT | WNOHANG | WEXITED) != 0) {
                        auto _errno = errno;

                        if (_errno == ECHILD) {
                            Log::notice("no child process is running");
                            terminate = true;
                            break;
                        }

                        throw std::system_error(_errno, std::system_category(), "wait failed");
                    }

                    auto pid = siginfo.si_pid;
                    // WNOHANG: zero out the si_pid field before the call and check for a nonzero value in this field after the call
                    if (pid == 0)
                        break;

                    Log::debug("child process exits (PID=%i)", pid);

                    Service service;
                    if (services.TryGetService(pid, &service)) {
                        if (service.CheckProcessState())
                            throw AtoneException("invalid service process state");

                        services.CheckService(service);
                    }
                    else {
                        Supervisor::ReapZombieProcess(pid);
                    }
                }
                
                break;
            }
            default:
                Log::debug("unhandled signal received: %s", strsignal(signum));
                break;
        }
    }

    // stop

    Log::trace("stopping all services");
    services.Stop();

    // TODO: kill all children

    Supervisor::ReapZombieProcess();

    Log::info("exit (PID=%i)", getpid());
    return 0;
}

int main(int argc, char **argv) {

    // GLOBAL EXCEPTION HANDLING
    try {

        // call Atone's Main Program
        return atone_main(argc, argv);

    }
    catch (char const *message) {
        Log::fatal(message);
        std::cerr << "Caught exception: " << message << std::endl;
    }
    catch (const char *message) {
        Log::fatal(message);
        std::cerr << "Caught exception: " << message << std::endl;
    }
    catch (const std::string ex) {
        auto message = ex.c_str();
        Log::fatal(message);
        std::cerr << "Caught exception: " << message << std::endl;
    }
    catch (std::exception const &ex) {
        Log::fatal(ex.what());
        std::cerr << "Caught exception: " << ex.what() << std::endl;
    }
    catch (...) {
        auto name = typeid(std::current_exception()).name();
        Log::fatal("Caught unknown exception: %s", name);
        std::cerr << "Caught unknown exception: " << name << std::endl;
    }

    exit(3);
}
