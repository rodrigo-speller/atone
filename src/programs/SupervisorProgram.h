// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "ProgramBase.h"

#include "config/AtoneOptions.h"

namespace Atone {
    class SupervisorProgram: public ProgramBase {
    public:
        int Run(AtoneOptions &options);
    };
}
