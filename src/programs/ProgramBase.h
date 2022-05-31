// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "config/AtoneOptions.h"

namespace Atone {
    class ProgramBase {
    public:
        virtual int Run(AtoneOptions &options) = 0;
    };
}
