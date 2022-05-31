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
        std::string workdir;

        Context(ServicesManager services, std::string workdir);
        static Context FromOptions(AtoneOptions options);
    };
}
