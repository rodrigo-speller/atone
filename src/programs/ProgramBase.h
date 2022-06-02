// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "config/AtoneOptions.h"

namespace Atone {
    class ProgramBase {
    protected:
        ProgramBase(const AtoneOptions &options);
        const AtoneOptions &options;
    public:
        virtual int Run() = 0;
    };
}
