// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

#include "service/ServicesManager.h"

namespace Atone {

    class Supervisor {
        private:

            static Supervisor *instance;

            /** Signals handled by Atone's WaitSignal method. These signal are blocked*/
            sigset_t atoneSigset = sigset_t();
            /** Signals to restore after process fork. */
            sigset_t spawnSigset = sigset_t();

            Supervisor();
            ~Supervisor();
            static void RequireInstance();

            static void UnexpectedSignalHandler(int signum);

        public:
            static void Initialize();
            static void Dispose();

            static void ReapZombieProcess(const pid_t pid = -1, int *wstatus = nullptr);
            static pid_t Spawn(char *const argv[]);
            static pid_t CheckForExitedProcess(const pid_t pid = -1);
            static pid_t CheckForExitedProcess(const pid_t pid, const timespec &timeout);
            static bool SendSignal(const pid_t pid, const int signum);
            static int WaitSignal(siginfo_t *info);
            static int WaitSignal(siginfo_t *info, const timespec &timeout);
    };

}
