// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "ServiceConfig.h"

#include "yaml-cpp/yaml.h"
#include <string.h>
#include <wordexp.h>

#include "ServiceRestartMode.h"
#include "exception/AtoneException.h"

namespace Atone {

    ServiceConfig::ServiceConfig(std::string name) { this->name = name; }
    ServiceConfig::~ServiceConfig() { FreeCommandArgs(); }

    ServiceConfig *ServiceConfig::FromConfig(std::string name, YAML::Node config) {
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

        svcConfig->SetCommandArgs(config["command"]);
        svcConfig->SetDependsOn(config["depends_on"]);
        svcConfig->SetRestartMode(config["restart"]);

        return svcConfig;
    }

    void ServiceConfig::SetCommandArgs(YAML::Node command) {
        switch (command.Type()) {
            case YAML::NodeType::Undefined:
                throw AtoneException("service must have a command");

            case YAML::NodeType::Scalar:
                SetCommandArgs(command.as<std::string>());
                return;

            default:
                throw AtoneException("invalid service command");
        }
    }

    void ServiceConfig::SetCommandArgs(std::string command) {
        wordexp_t we_args;

        if (wordexp(command.c_str(), &we_args, 0) != 0) {
            throw AtoneException("invalid service command");
        }

        SetCommandArgs(we_args.we_wordc, we_args.we_wordv);
        wordfree(& we_args);
    }

    void ServiceConfig::SetCommandArgs(size_t argc, char ** argv) {
        FreeCommandArgs();

        char **dst_argv = new char *[argc + 1];

        for (size_t i = 0; i < argc; i++) {
            const auto src_arg = argv[i];
            const auto len = strlen(src_arg);
            dst_argv[i] = strcpy(new char[len + 1], src_arg);
        }
        dst_argv[argc] = NULL;

        this->argv = dst_argv;
        this->argc = argc;
    }

    void ServiceConfig::FreeCommandArgs() {
        const auto argv = this->argv;
        if (argv != NULL) {
            const auto argc = this->argc;

            this->argv = NULL;
            this->argc = 0;

            for (size_t i = 0; i < argc; i++) {
                delete argv[i];
            }
            delete argv;
        }
    }

    void ServiceConfig::SetDependsOn(YAML::Node depends_on) {
        switch (depends_on.Type()) {
            case YAML::NodeType::Null:
            case YAML::NodeType::Undefined:
                return;

            case YAML::NodeType::Scalar:
                this->depends_on = std::vector<std::string>{depends_on.as<std::string>()};
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

        this->depends_on = value;
    }

    void ServiceConfig::SetRestartMode(YAML::Node restart) {
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

        this->restart = value;
    }

}