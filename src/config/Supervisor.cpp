// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include <sys/wait.h>
#include <unistd.h>

#include "Supervisor.h"
#include "logging/Log.h"

namespace Atone {
    Supervisor *Supervisor::instance = new Supervisor();

    Supervisor::Supervisor() {

        // Atone must handle all possible signals
        if (sigfillset(&atoneSigset)) {
            Log::crit("sigfillset failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        };

        // blocks Atone's signals
        if (sigprocmask(SIG_BLOCK, &atoneSigset, &spawnSigset)) {
            Log::crit("sigprocmask failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        };

        for (int i = 1; i <= 31; i++) {
            // maps the unexpected signal handler to log any signal handling leakage
            signal(i, UnexpectedSignalHandler);
        }

    }

    void Supervisor::UnexpectedSignalHandler(int signum) {
        Log::crit("unexpected signal received: %s", strsignal(signum));
    }

    void Supervisor::ReapZombieProcess(pid_t pid) {
        Log::trace("reaping zombie process(es) (PID=%i)", pid);

        while (true) {

            switch (auto wpid = waitpid(pid, NULL, WNOHANG)) {
                // on error, -1 is returned
                case -1: {
                    auto err = errno;

                    // avoid warning "No child processes"
                    if (err == ECHILD) {
                        Log::debug("no zombie process to reap");
                        return;
                    }

                    Log::warn("zombie process reaping failed: %s", strerror(err));
                    return;
                }

                // one or more child(ren) specified by pid exist, but have not yet changed state, then 0 is returned
                case 0: {
                    Log::debug("no zombie process to reap, but has one or more running children");
                    return;
                }

                // on success, returns the process ID of the child whose state has changed
                default: {
                    Log::info("zombie process reaped (PID=%i)", wpid);
                    break;
                }
            }
        }
    }

    pid_t Supervisor::Spawn(const char *service_name, char *const argv[]) {
        auto pid = vfork();
        
        if (pid < 0) { // error
            auto _errno = errno;
            Log::crit("%s: service fork failed: %s", service_name, strerror(_errno));
            throw std::system_error(_errno, std::system_category(), "fork failed");
        }

        if (pid == 0) { // child process

            // NOTE: A child process created via fork(2) inherits the process signal mask from the parent.
            // Before execve(2) we must restores sigprocmask.
            if (sigprocmask(SIG_SETMASK, &instance->spawnSigset, NULL)) {
                Log::crit("sigprocmask failed: %s", strerror(errno));
                exit(EXIT_FAILURE);
            }

            // NOTE: A child created via fork(2) inherits a copy of its parent's signal dispositions.
            // During an execve(2), the dispositions of handled signals are reset to the default;
            // the dispositions of ignored signals are left unchanged.

            // execute
            execvp(argv[0], argv);

            // execution must not return, except on error
            Log::crit("%s: service exec failed: %s", service_name, strerror(errno));
            exit(EXIT_FAILURE);

        }

        return pid;
    }

    int Supervisor::WaitSignal(siginfo_t *info) {
        int signum;

        if ((signum = sigwaitinfo(&instance->atoneSigset, info)) == -1) {
            Log::crit("sigwait failed: %s", strerror(errno));
            return 0;
        }

        return signum;
    }

}