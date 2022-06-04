// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "ProgramFactory.h"

#include "config/AtoneMode.h"
#include "config/AtoneOptions.h"
#include "programs/HelpProgram.h"
#include "programs/ProgramBase.h"
#include "programs/SupervisorProgram.h"
#include "programs/VersionProgram.h"

namespace Atone {
    std::shared_ptr<ProgramBase> ProgramFactory::CreateProgram(const AtoneOptions &options) {
        ProgramBase *program;

        switch (options.mode) {
        case AtoneMode::Help:
            program = new HelpProgram(options);
            break;
        case AtoneMode::Version:
            program = new VersionProgram(options);
            break;
        case AtoneMode::SingleService:
        case AtoneMode::MultiServices:
            program = new SupervisorProgram(options);
            break;
        default:
            throw std::domain_error("invalid mode");
        }

        return std::shared_ptr<ProgramBase>(program);
    }
}
