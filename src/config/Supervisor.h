// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <signal.h>

#include "ServicesManager.h"

namespace Atone {

    class Supervisor {
        private:

            static Supervisor *instance;

            /** Signals handled by Atone's WaitSignal method. These signal are blocked*/
            sigset_t atoneSigset = sigset_t();
            /** Signals to restore after process fork. */
            sigset_t spawnSigset = sigset_t();

            Supervisor();
            static void UnexpectedSignalHandler(int signum);

        public:

            static void ReapZombieProcess(pid_t pid = -1);
            static pid_t Spawn(const char *service_name, char *const argv[]);
            static int WaitSignal(siginfo_t *info);
    };

}
