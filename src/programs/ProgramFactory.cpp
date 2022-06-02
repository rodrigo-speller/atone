// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "ProgramFactory.h"

#include "config/AtoneOptions.h"
#include "programs/HelpProgram.h"
#include "programs/ProgramBase.h"
#include "programs/SupervisorProgram.h"
#include "programs/VersionProgram.h"

namespace Atone {
    std::shared_ptr<ProgramBase> ProgramFactory::CreateProgram(const AtoneOptions &options) {
        ProgramBase *program;

        if (options.usage) {
            program = new HelpProgram(options);
        } else if (options.version) {
            program = new VersionProgram(options);
        } else {
            program = new SupervisorProgram(options);
        }

        return std::shared_ptr<ProgramBase>(program);
    }
}
