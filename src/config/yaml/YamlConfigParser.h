// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <string>

#include "yaml-cpp/yaml.h"

#include "config/Context.h"
#include "config/ServiceConfig.h"
#include "config/ServicesManager.h"

namespace Atone {
    class YamlConfigParser {
    public:
        Context ParseDocument(std::string path);
        Context ParseDocument(YAML::Node document, std::string basepath);
        ServicesManager ParseServices(YAML::Node document);
        Service ParseService(std::string name, YAML::Node node);

        void SetCommandArgs(ServiceConfig *target, YAML::Node command);
        void SetDependsOn(ServiceConfig *target, YAML::Node depends_on);
        void SetRestartMode(ServiceConfig *target, YAML::Node restart);
    };
}
