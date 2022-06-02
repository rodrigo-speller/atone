// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <string>

#include "config/AtoneOptions.h"
#include "config/ServicesManager.h"

namespace Atone {
    class Context {
    public:
        ServicesManager services;
        const std::string &workdir;

        Context(const ServicesManager &services, const std::string &workdir);
        static Context FromOptions(const AtoneOptions &options);
    };
}
