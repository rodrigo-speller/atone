// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

#include "yaml-cpp/yaml.h"

#include "config/Context.h"
#include "config/ServiceConfig.h"
#include "service/ServicesManager.h"

namespace Atone {
    class YamlConfigParser {
    public:
        Context ParseDocument(const std::string &path);
        Context ParseDocument(const YAML::Node &document, const std::string &basepath);
        ServicesManager ParseServices(const YAML::Node &document);
        ServiceConfig ParseService(const std::string &name, const YAML::Node &node);

        void SetCommandArgs(ServiceConfig &target, const YAML::Node &command);
        void SetDependsOn(ServiceConfig &target, const YAML::Node &depends_on);
        void SetRestartMode(ServiceConfig &target, const YAML::Node &restart);
    };
}
