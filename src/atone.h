// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include <time.h>

#include "config/ServicesManager.h"

namespace Atone {
    int main(int argc, char **argv);

    bool reap_processes(ServicesManager &services, bool restart = false);

    void kill_all_process(ServicesManager &services, timespec timeout);
    bool stop_all_services(ServicesManager &services, timespec timeout);

    void PrintUsage(char *program);
    void PrintVersion();
}
