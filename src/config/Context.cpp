// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "Context.h"

#include <filesystem>
#include <libgen.h>
#include <memory>
#include <string>

#include "config/AtoneOptions.h"
#include "config/Context.h"
#include "config/Service.h"
#include "config/ServiceConfig.h"
#include "config/ServicesManager.h"
#include "config/yaml/YamlConfigParser.h"
#include "exception/AtoneException.h"

namespace Atone {
    Context::Context(ServicesManager services, std::string workdir)
        : services(services), workdir(workdir) {
    }

    Context Context::FromOptions(AtoneOptions options) {
        switch (options.mode) {
            case AtoneMode::SingleService: {
                auto service_cfg = std::make_shared<ServiceConfig>(options.serviceName);
                service_cfg->SetCommandArgs(options.commandArgc, options.commandArgv);
                auto service = Service(service_cfg);
                auto services = ServicesManager(service);
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