// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "VersionProgram.h"

#include "config/AtoneOptions.h"
#include "utils/constants.h"

namespace Atone {
    VersionProgram::VersionProgram(const AtoneOptions &options)
        : ProgramBase(options) {
    }

    int VersionProgram::Run() {
        PrintVersion();
        return 0;
    }

    void VersionProgram::PrintVersion() {
        std::cout << ATONE_BUILD_VERSION_STAMP << "\n";
    }
}
