// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "ProgramBase.h"

#include "config/AtoneOptions.h"

namespace Atone {
    class HelpProgram: public ProgramBase {
    private:
        void PrintUsage(char *program);
    public:
        int Run(AtoneOptions &options);
    };
}
