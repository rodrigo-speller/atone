// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "YamlConfigParser.h"

#include <filesystem>
#include <map>
#include <string>

#include "yaml-cpp/yaml.h"

#include "config/Context.h"
#include "config/Service.h"
#include "config/ServiceConfig.h"
#include "config/ServicesManager.h"
#include "exception/AtoneException.h"

namespace Atone {
    Context YamlConfigParser::ParseDocument(std::string configFile) {
        auto config_file_path = std::filesystem::path(configFile);
        auto config_path = config_file_path.parent_path();

        auto config = YAML::LoadFile(configFile);
        return ParseDocument(config, config_path);
    }
    
    Context YamlConfigParser::ParseDocument(YAML::Node document, std::string basepath) {
        auto settings_node = document["settings"];
        std::string workdir;

        if (settings_node.Type() == YAML::NodeType::Map) {
            auto workdir_node = settings_node["workdir"];

            if (workdir_node.Type() == YAML::NodeType::Scalar) {
                workdir = basepath.append(workdir_node.as<std::string>());
            }
        }

        auto services = ParseServices(document);
        return Context(services, workdir);
    }
    
    ServicesManager YamlConfigParser::ParseServices(YAML::Node document) {
        const auto services_node = document["services"];

        if (!services_node) {
            throw std::domain_error("services not found");
        }

        if (services_node.Type() != YAML::NodeType::Map) {
            throw std::domain_error("invalid services section value");
        }

        auto services = std::map<std::string, Service>();

        for (auto entry : services_node) {
            auto service_name = entry.first.as<std::string>();
            auto service = ParseService(service_name, entry.second);

            services.insert(std::pair(service_name, service));
        }

        return ServicesManager(services);
    }

    Service YamlConfigParser::ParseService(std::string name, YAML::Node config) {
        auto svcConfig = new ServiceConfig(name);

        switch (config.Type()) {
            case YAML::NodeType::Scalar:
                svcConfig->SetCommandArgs(config.as<std::string>());
                return svcConfig;

            case YAML::NodeType::Map:
                break;

            default:
                throw AtoneException("invalid service configuration");
        }

        SetCommandArgs(svcConfig, config["command"]);
        SetDependsOn(svcConfig, config["depends_on"]);
        SetRestartMode(svcConfig, config["restart"]);

        return Service(svcConfig);
    }

    void YamlConfigParser::SetCommandArgs(ServiceConfig* target, YAML::Node command) {
        switch (command.Type()) {
            case YAML::NodeType::Undefined:
                throw AtoneException("service must have a command");

            case YAML::NodeType::Scalar:
                target->SetCommandArgs(command.as<std::string>());
                return;

            default:
                throw AtoneException("invalid service command");
        }
    }

    void YamlConfigParser::SetDependsOn(ServiceConfig* target, YAML::Node depends_on) {
        switch (depends_on.Type()) {
            case YAML::NodeType::Null:
            case YAML::NodeType::Undefined:
                return;

            case YAML::NodeType::Scalar:
                target->depends_on = std::vector<std::string>{depends_on.as<std::string>()};
                return;

            case YAML::NodeType::Sequence:
                break;

            default:
                throw AtoneException("invalid depends_on");
        }

        size_t sz = depends_on.size();
        auto value = std::vector<std::string>(sz);
        for (size_t i = 0; i < sz; i++) {
            auto node = depends_on[i];

            if (node.Type() != YAML::NodeType::Scalar)
                throw AtoneException("invalid depends_on");

            value[i] = node.as<std::string>();
        }

        target->depends_on = value;
    }

    void YamlConfigParser::SetRestartMode(ServiceConfig* target, YAML::Node restart) {
        switch (restart.Type()) {
            case YAML::NodeType::Null:
            case YAML::NodeType::Undefined:
                return;

            case YAML::NodeType::Scalar:
                break;

            default:
                throw AtoneException("invalid restart");
        }

        auto str_value = restart.as<std::string>();

        ServiceRestartMode value;
        if (str_value == "no")
            value = ServiceRestartMode::No;
        else if (str_value == "always")
            value = ServiceRestartMode::Always;
        else if (str_value == "on-failure")
            value = ServiceRestartMode::OnFailure;
        else if (str_value == "unless-stopped")
            value = ServiceRestartMode::UnlessStopped;
        else
            throw AtoneException("invalid restart");

        target->restart = value;
    }
}
