// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

#include "config/AtoneOptions.h"
#include "service/ServicesManager.h"

namespace Atone {
    class Context {
    public:
        ServicesManager *services;
        const string workdir;

        Context();
        Context(const string &workdir);
        static Context FromOptions(const AtoneOptions &options);
    };
}
