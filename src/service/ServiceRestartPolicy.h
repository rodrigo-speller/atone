// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

namespace Atone {

    enum class ServiceRestartPolicy {
        Never,
        Always,
        OnFailure,
        UnlessStopped,
    };

}