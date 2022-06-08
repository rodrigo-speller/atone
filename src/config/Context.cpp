// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "Context.h"

#include "config/AtoneOptions.h"
#include "config/Context.h"
#include "config/ServiceConfig.h"
#include "config/yaml/YamlConfigParser.h"
#include "exception/AtoneException.h"
#include "service/Service.h"
#include "service/ServicesManager.h"
#include "logging/Log.h"

namespace Atone {
    Context::Context()
      : Context(string()) {
    }

    Context::Context(const std::string &workdir)
        : services(new ServicesManager()), workdir(workdir) {
    }

    Context Context::FromOptions(const AtoneOptions &options) {
        switch (options.mode) {
            case AtoneMode::SingleService: {
                auto service_cfg = ServiceConfig(options.serviceName);
                service_cfg.SetCommandArgs(options.commandArgc, options.commandArgv);

                Context context;
                context.services->AddService(service_cfg);
                return context;
            }

            case AtoneMode::MultiServices: {
                return YamlConfigParser().ParseDocument(options.configFile);
            }

            default: {
                throw AtoneException("unexpected mode");
            }
        }
    }
}