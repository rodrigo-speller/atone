// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "Context.h"

#include <filesystem>
#include <libgen.h>

#include "exception/AtoneException.h"

namespace Atone {

    Context::Context(AtoneOptions options) {

        ServicesManager services;

        switch (options.mode) {

            case AtoneMode::SingleService: {
                auto service_cfg = new ServiceConfig(options.serviceName);
                service_cfg->SetCommandArgs(options.argc, options.argv);
                auto service = Service(service_cfg);
                services = ServicesManager(service);
                break;
            }

            case AtoneMode::MultiServices: {
                auto config_file_opt = options.configFile;
                auto config_file_path = std::filesystem::path(options.configFile);
                auto config_path = config_file_path.parent_path();

                auto config = YAML::LoadFile(options.configFile);

                auto settings_node = config["settings"];

                if (settings_node.Type() == YAML::NodeType::Map) {
                    auto workdir_node = settings_node["workdir"];

                    if (workdir_node.Type() == YAML::NodeType::Scalar) {
                        this->workdir = config_path.append(workdir_node.as<std::string>());
                    }
                }

                services = ServicesManager(config);

                break;
            }

            default: {
                throw AtoneException("unexpected mode");
            }

        }

        this->services = services;

    }

}