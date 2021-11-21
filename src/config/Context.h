// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <string>

#include "AtoneOptions.h"
#include "ServicesManager.h"

namespace Atone {

    class Context {
        public:
            ServicesManager services;
            std::string workdir;

            Context(AtoneOptions options);
    };

}
