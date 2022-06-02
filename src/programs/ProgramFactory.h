// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "config/AtoneOptions.h"
#include "programs/ProgramBase.h"

namespace Atone {
    class ProgramFactory {
    public:
        static std::shared_ptr<ProgramBase> CreateProgram(const AtoneOptions &options);
    };
}
