// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "Context.h"

#include <filesystem>
#include <libgen.h>
#include <string>

#include "config/AtoneOptions.h"
#include "config/Context.h"
#include "config/Service.h"
#include "config/ServiceConfig.h"
#include "config/ServicesManager.h"
#include "config/yaml/YamlConfigParser.h"
#include "exception/AtoneException.h"

namespace Atone {
    Context::Context(const ServicesManager &services, const std::string &workdir)
        : services(services), workdir(workdir) {
    }

    Context Context::FromOptions(const AtoneOptions &options) {
        switch (options.mode) {
            case AtoneMode::SingleService: {
                auto service_cfg = ServiceConfig(options.serviceName);
                service_cfg.SetCommandArgs(options.commandArgc, options.commandArgv);
                auto services = ServicesManager();
                services.Add(service_cfg);

                return Context(services, 0);
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