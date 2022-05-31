// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "ProgramBase.h"

#include "config/AtoneOptions.h"
#include "config/ServicesManager.h"

namespace Atone {
    class SupervisorProgram: public ProgramBase {
    private:
        bool ReapProcesses(ServicesManager &services, bool restart = false);
        void KillAllProcess(ServicesManager &services, timespec timeout);
        bool StopAllServices(ServicesManager &services, timespec timeout);
    public:
        int Run(AtoneOptions &options);
    };
}
