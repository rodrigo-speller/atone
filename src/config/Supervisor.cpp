// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "Supervisor.h"

#include "logging/Log.h"
#include "utils/time.h"

namespace Atone {
    Supervisor *Supervisor::instance = nullptr;

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

    Supervisor::~Supervisor() {
        // restores the signals blocked by Atone
        if (sigprocmask(SIG_SETMASK, &spawnSigset, nullptr)) {
            Log::crit("sigprocmask failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        };
    }

    void Supervisor::Initialize() {
        if (instance == nullptr) {
            Log::debug("supervisor: initializing...");
            instance = new Supervisor();
            Log::debug("supervisor: ready");
        }
        else {
            Log::warn("supervisor: already initialized");
        }
    }

    void Supervisor::Dispose() {
        if (instance != nullptr) {
            Log::debug("supervisor: disposing...");
            delete instance;
            instance = nullptr;
            Log::debug("supervisor: disposed");
        }
        else {
            Log::warn("supervisor: already disposed");
        }
    }

    void Supervisor::RequireInstance() {
        if (instance == nullptr) {
            throw std::domain_error("supervisor is not initialized");
        }
    }

    void Supervisor::UnexpectedSignalHandler(int signum) {
        Log::crit("unexpected signal received: %s", strsignal(signum));
    }

    void Supervisor::ReapZombieProcess(pid_t pid) {
        RequireInstance();

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

    pid_t Supervisor::Spawn(char *const argv[]) {
        RequireInstance();

        auto pid = vfork();
        
        if (pid < 0) { // error
            auto _errno = errno;
            Log::crit("service fork failed", strerror(_errno));
            throw std::system_error(_errno, std::system_category(), "fork failed");
        }

        if (pid == 0) { // child process

            // release resources

            // NOTE: A child process created via fork(2) inherits the process signal mask from the parent.
            // Before execve(2) we must restores sigprocmask.

            // NOTE: A child created via fork(2) inherits a copy of its parent's signal dispositions.
            // During an execve(2), the dispositions of handled signals are reset to the default;
            // the dispositions of ignored signals are left unchanged.

            instance->Dispose();

            // execute
            execvp(argv[0], argv);

            // execution must not return, except on error
            Log::crit("service exec failed", strerror(errno));
            exit(EXIT_FAILURE);

        }

        return pid;
    }

    /**
     * Check if any child process has exited.
     * @return Returns the pid of the child process that has exited,
     *         or 0 if no child process has exited (but one or more child processes still running),
     *         or -1 if no child process is running.
     */
    pid_t Supervisor::CheckForExitedProcess() {
        siginfo_t siginfo;

        // WNOHANG: zero out the si_pid field before the call and check for a nonzero value in this field after the call
        siginfo.si_pid = 0;
        if (waitid(P_ALL, 0, &siginfo, WNOWAIT | WNOHANG | WEXITED) != 0) {
            auto _errno = errno;

            if (_errno == ECHILD) {
                Log::notice("no child process is running");
                return -1;
            }

            throw std::system_error(_errno, std::system_category(), "wait failed");
        }

        auto pid = siginfo.si_pid;
        assert(pid >= 0);
        return pid;
    }


    /**
     * Wait for a signal.
     * 
     * @param info If the info argument is not NULL, then the buffer that it
     *             points to is used to return a structure of type siginfo_t
     *             containing information about the signal.
     * @return Returns the signal number, or -1 if the call fail.
     */
    int Supervisor::WaitSignal(siginfo_t *info) {
        RequireInstance();

        int signum;

        if ((signum = sigwaitinfo(&instance->atoneSigset, info)) == -1) {
            Log::crit("sigwait failed: %s", strerror(errno));
            return -1;
        }

        return signum;
    }

    /**
     * Wait for a signal.
     * 
     * @param info If the info argument is not NULL, then the buffer that it
     *             points to is used to return a structure of type siginfo_t
     *             containing information about the signal.
     * @param timeout Specifies a minimum interval for which the thread is
     *                suspended waiting for a signal
     * @return Returns the signal number, or 0 if the operation was timed-out, or -1 if the call fail.
     */
    int Supervisor::WaitSignal(siginfo_t *info, const timespec &timeout) {
        RequireInstance();

        int signum;

        timespec now;
        timespec _timeout;
        clock_gettime(CLOCK_MONOTONIC, &now);
        if (timespec_diff(now, timeout, _timeout) == -1) {
            _timeout.tv_sec = 0;
            _timeout.tv_nsec = 0;
        }

        if ((signum = sigtimedwait(&instance->atoneSigset, info, &_timeout)) == -1) {
            auto err = errno;

            if (err == EAGAIN) {
                // timeout
                return 0;
            }

            Log::crit("sigwait failed: %s", strerror(err));
            return -1;
        }

        return signum;
    }

}