// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "YamlConfigParser.h"

#include "config/Context.h"
#include "config/ServiceConfig.h"
#include "exception/AtoneException.h"
#include "service/Service.h"
#include "service/ServicesManager.h"

namespace Atone {
    Context YamlConfigParser::ParseDocument(const std::string &configFile) {
        auto config_file_path = std::filesystem::path(configFile);
        auto config_path = config_file_path.parent_path();

        auto config = YAML::LoadFile(configFile);
        return ParseDocument(config, config_path);
    }
    
    Context YamlConfigParser::ParseDocument(const YAML::Node &document, const std::string &basepath) {
        auto settings_node = document["settings"];
        std::string workdir;

        if (settings_node && settings_node.Type() == YAML::NodeType::Map) {
            auto workdir_node = settings_node["workdir"];

            if (workdir_node.Type() == YAML::NodeType::Scalar) {
                workdir = basepath + "/" + workdir_node.as<std::string>();
            }
        }

        auto context = Context(workdir);
        auto services = ParseServices(document);
        for (auto service : services) {
            context.services->AddService(service);
        }

        return context;
    }
    
    list<ServiceConfig> YamlConfigParser::ParseServices(const YAML::Node &document) {
        const auto services_node = document["services"];

        if (!services_node) {
            throw std::domain_error("services not found");
        }

        if (services_node.Type() != YAML::NodeType::Map) {
            throw std::domain_error("invalid services section value");
        }

        list<ServiceConfig> services;
        for (auto entry : services_node) {
            auto service_name = entry.first.as<std::string>();
            auto service = ParseService(service_name, entry.second);
            services.push_back(service);
        }

        return services;
    }

    ServiceConfig YamlConfigParser::ParseService(const std::string &name, const YAML::Node &config) {
        auto svcConfig = ServiceConfig(name);

        switch (config.Type()) {
            case YAML::NodeType::Scalar:
                svcConfig.SetCommandArgs(config.as<std::string>());
                return svcConfig;

            case YAML::NodeType::Map:
                break;

            default:
                throw AtoneException("invalid service configuration");
        }

        auto command = config["command"];
        if (command) SetCommandArgs(svcConfig, command);

        auto depends_on = config["depends_on"];
        if (depends_on) SetDependsOn(svcConfig, depends_on);

        auto restart = config["restart"];
        if (restart) SetRestartPolicy(svcConfig, restart);

        return svcConfig;
    }

    void YamlConfigParser::SetCommandArgs(ServiceConfig &target, const YAML::Node &command) {
        switch (command.Type()) {
            case YAML::NodeType::Null:
            case YAML::NodeType::Undefined:
                throw AtoneException("service must have a command");

            case YAML::NodeType::Scalar:
                target.SetCommandArgs(command.as<std::string>());
                return;

            default:
                throw AtoneException("invalid service command");
        }
    }

    void YamlConfigParser::SetDependsOn(ServiceConfig &target, const YAML::Node &depends_on) {
        switch (depends_on.Type()) {
            case YAML::NodeType::Null:
            case YAML::NodeType::Undefined:
                target.depends_on = std::vector<std::string>();
                return;

            case YAML::NodeType::Scalar:
                target.depends_on = std::vector<std::string>{depends_on.as<std::string>()};
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

        target.depends_on = value;
    }

    void YamlConfigParser::SetRestartPolicy(ServiceConfig &target, const YAML::Node &restart) {
        switch (restart.Type()) {
            case YAML::NodeType::Null:
            case YAML::NodeType::Undefined:
                target.restart = ServiceRestartPolicy::No;
                return;

            case YAML::NodeType::Scalar:
                break;

            default:
                throw AtoneException("invalid restart policy");
        }

        auto str_value = restart.as<std::string>();

        ServiceRestartPolicy value;
        if (str_value == "no")
            value = ServiceRestartPolicy::No;
        else if (str_value == "always")
            value = ServiceRestartPolicy::Always;
        else if (str_value == "on-failure")
            value = ServiceRestartPolicy::OnFailure;
        else if (str_value == "unless-stopped")
            value = ServiceRestartPolicy::UnlessStopped;
        else
            throw AtoneException("invalid restart policy");

        target.restart = value;
    }
}
