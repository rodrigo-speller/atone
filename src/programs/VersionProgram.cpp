// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "VersionProgram.h"

#include <iostream>

#include "config/AtoneOptions.h"
#include "utils/constants.h"

namespace Atone {
    int VersionProgram::Run(AtoneOptions &options) {
        PrintVersion();
        return 0;
    }

    void VersionProgram::PrintVersion() {
        std::cout << ATONE_BUILD_VERSION_STAMP << "\n";
    }
}
