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

    /**
     * Reaps zombie processes.
     * @param pid Process ID to reap. The semantics of pid are the same
     *            as waitpid.
     *            If less than -1, reap any zombie process that group ID
     *            is equal to the absolute value of pid.
     *            If pid is -1, reap all zombie processes.
     *            If pid is 0, reap all zombie processes that group ID
     *            is equal to the process group ID of the calling
     *            process.
     *            If pid is greater than 0, reap the zombie process with
     *            process ID equals to pid.
     * @param wstatus Pointer to a status variable to store the exit
     *                status of the last reaped process. The semantics
     *                of status are the same as waitpid.
     *                If status is NULL, the exit status is not stored.
     * @return Returns true, if any process was reaped, or no process
     *         matches pid's criteria. Returns false, if one or more
     *         processes matches pid's criteria, but have not yet
     *         changed to be reaped.
     */
    bool Supervisor::ReapZombieProcess(pid_t pid, int *status) {
        RequireInstance();

        Log::trace("reaping zombie process(es) (PID=%i)", pid);

        while (true) {
            int wstatus;
            switch (auto wpid = waitpid(pid, &wstatus, WNOHANG)) {
                // on error, -1 is returned
                case -1: {
                    auto err = errno;

                    // avoid warning "No child processes"
                    if (err == ECHILD) {
                        Log::debug("no zombie process to reap");
                        return true;
                    }

                    throw std::runtime_error(std::string("waitpid failed: ") + strerror(err));
                }

                // one or more child(ren) specified by pid exist, but have not yet changed state, then 0 is returned
                case 0: {
                    Log::debug("no zombie process to reap, but has one or more running children");
                    return false;
                }

                // on success, returns the process ID of the child whose state has changed
                default: {
                    assert(wpid > 0);
                    Log::info("zombie process reaped (PID=%i)", wpid);

                    if (status != nullptr) {
                        *status = wstatus;
                    }

                    if (wpid == pid) {
                        return true;
                    }

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

            // NOTE: Until that point, the child shares all memory with its
            // parent, including the stack.

            // NOTE: A child process created via fork(2) inherits the process signal mask from the parent.
            // Before execve(2) we must restores sigprocmask.

            // NOTE: A child created via fork(2) inherits a copy of its parent's signal dispositions.
            // During an execve(2), the dispositions of handled signals are reset to the default;
            // the dispositions of ignored signals are left unchanged.

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
     * @param pid The pid of the child process to check, like pid paramenter on waitpid.
     * @return Returns the pid of the child process that has exited,
     *         or 0 if no child process has exited (but one or more child processes still running),
     *         or -1 if no child process is running.
     */
    pid_t Supervisor::CheckForExitedProcess(const pid_t pid) {

        // NOTE: simulates waitpid(2) behavior for pid value
        id_t id;
        idtype_t idtype;
        switch (pid){
        case -1:
            idtype = P_ALL;
            id = 0;
            break;
        case 0:
            idtype = P_PGID;
            id = getpgid(0);
        default:
            if (pid < 0) {
                idtype = P_PGID;
                id = -pid;
            }
            else {
                idtype = P_PID;
                id = pid;
            }
            break;
        }

        auto result = waitid(idtype, id, NULL, WNOWAIT | WNOHANG | WEXITED);

        if (result < 0) {
            auto _errno = errno;

            if (_errno == ECHILD) {
                Log::notice("no child process is running");
                return -1;
            }

            throw std::system_error(_errno, std::system_category(), "wait failed");
        }

        return result;
    }

    /**
     * Check if any child process has been exited until a timeout expires.
     * @param pid The pid of the child process to check, like pid paramenter on waitpid.
     * @param timeout The timeout.
     * @return Returns the pid of the child process that has exited,
     *         or 0 if no child process has exited (but one or more child processes still running),
     *         or -1 if no child process is running.
     */
    pid_t Supervisor::CheckForExitedProcess(const pid_t pid, const timespec &timeout) {
        while (true) {
            auto result = CheckForExitedProcess(pid);

            if (result == 0) {
                if (timeout_expired(timeout)) {
                    return 0;
                }

                std::this_thread::yield();
                continue;
            }

            return result;
        }
    }

    /**
     * Send a signal to a child process.
     * 
     * @param pid The pid of the child process to send the signal to.
     * @param signum The signal number to send.
     * @return Returns true if the signal was sent successfully, false if the process was not found.
     */
    bool Supervisor::SendSignal(const pid_t pid, const int signum) {
        if (kill(pid, signum) == 0) {
            Log::debug("signal sent (PID=%i, signal=%i)", pid, signum);
            return true;
        }

        auto _errno = errno;
        if (_errno == ESRCH) {
            Log::debug("no process found (PID=%i)", pid);
            return false;
        }

        throw std::system_error(_errno, std::system_category(), "send sigal failed");
    }

    /**
     * Wait for a signal.
     * 
     * @param info If the info argument is not NULL, then the buffer that it
     *             points to is used to return a structure of type siginfo_t
     *             containing information about the signal.
     * @return Returns the signal number.
     */
    int Supervisor::WaitSignal(siginfo_t *info) {
        RequireInstance();

        int signum;

        if ((signum = sigwaitinfo(&instance->atoneSigset, info)) == -1) {
            auto err = errno;
            throw system_error(errno, system_category(), string("sigwait failed: ") + strerror(err));
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
     * @return Returns the signal number, or 0 if the operation was timed-out.
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

            throw system_error(err, system_category(), string("sigwait failed: ") + strerror(err));
        }

        return signum;
    }

}