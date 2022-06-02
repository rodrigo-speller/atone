// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "ProgramBase.h"

#include "config/AtoneOptions.h"

namespace Atone {
    class VersionProgram: public ProgramBase {
    private:
        void PrintVersion();

    public:
        VersionProgram(const AtoneOptions &options);
        int Run();
    };
}
