// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "Service.h"

#include "config/ServiceConfig.h"
#include "exception/AtoneException.h"
#include "logging/Log.h"
#include "service/ServicesManager.h"
#include "service/ServiceStatus.h"
#include "system/Supervisor.h"
#include "utils/time.h"

namespace Atone {
  Service::Service(const ServicesManager *manager, const ServiceConfig &config)
    : manager(manager), config(config) {
  }

  //////////////////////////////////////////////////////////////////////

  string Service::name() const { return config.name; }
  size_t Service::argc() const { return config.argc; }
  shared_ptr<char *> Service::argv() const { return config.argv; }
  vector<string> Service::dependsOn() const { return config.depends_on; }
  ServiceRestartMode Service::restartMode() const { return config.restart; }

  //////////////////////////////////////////////////////////////////////

  ServiceStatus Service::status() const { return state.status; }
  pid_t Service::pid() const { return state.pid; }
  int Service::exitCode() const { return state.exit_code; }

  //////////////////////////////////////////////////////////////////////

  bool Service::isRunning() const { return state.pid != 0; }

  bool Service::canRestart() const {
    if (isRunning()) {
      return false;
    }

    switch (restartMode()) {
      case ServiceRestartMode::No:
        return false;
      case ServiceRestartMode::Always:
        return true;
      case ServiceRestartMode::OnFailure:
        return (exitCode() != EXIT_SUCCESS);
        break;
      case ServiceRestartMode::UnlessStopped:
        return (status() != ServiceStatus::Stopped);
        break;
      default:
        throw domain_error("invalid restart mode");
    }
  }

  //////////////////////////////////////////////////////////////////////

  void Service::Start() {
    Start({});
  }

  void Service::Start(vector<Service*> callstack) {
    find(callstack.begin(), callstack.end(), this) != callstack.end()
      ? throw AtoneException("circular dependency")
      : callstack.push_back(this);

    // start dependencies
    for (auto dependency : GetDependencies()) {
      switch (dependency->status()) {
        case ServiceStatus::Running:
          break;
        case ServiceStatus::NotStarted:
            dependency->Start(callstack);
            break;
        case ServiceStatus::Stopped:
        case ServiceStatus::Broken:
          if (dependency->canRestart()) {
            dependency->Start(callstack);
          }
          else {
            throw domain_error("invalid dependency state");
          }
          break;
        default:
          throw domain_error("invalid dependency status");
      }
    }

    // start process
    Service::StartProcess();
  }

  void Service::StartProcess() {
    auto service_name = config.name.c_str();
    Log::info("%s: starting service", service_name);

    // check if the service and, eventually, its process are running
    if (CheckProcessState()) {
      Log::info("%s: service already running", service_name);
      return;
    }

    // spawn process

    pid_t pid;
    auto argv = this->argv();
    Log::debug("%s: spawning service", service_name);
    try {
      pid = Supervisor::Spawn(argv.get());
    }
    catch (...) {
      Log::error("%s: failed to spawn service", service_name);
      state.status = ServiceStatus::Broken;
      throw;
    }

    // update state
    state.pid = pid;
    state.status = ServiceStatus::Running;
    state.exit_code = 0;

    Log::notice("%s: service started (PID=%i)", service_name, pid);
  }

  //////////////////////////////////////////////////////////////////////

  bool Service::Stop() {
    auto service_name = config.name.c_str();
    Log::info("%s: stopping service", service_name);

    // check if the service and, eventually, its process are running
    if (!CheckProcessState()) {
      return true;
    }

    // send the SIGTERM signal to the service process
    auto pid = state.pid;
    Log::trace(
      "%s: sending signal to service process: %s",
      service_name, strsignal(SIGTERM)
    );
    Supervisor::SendSignal(pid, SIGTERM);

    // await for the process to exit
    // TODO: define a timeout
    timespec timeout = { 5, 0 };
    timeout_from_now(timeout);
    Supervisor::CheckForExitedProcess(pid, timeout);

    // checks the process state to reap it and update the service
    return !CheckProcessState();
  }

  //////////////////////////////////////////////////////////////////////

  bool Service::CheckProcessState() {
    auto service_name = config.name.c_str();
    Log::trace("%s: checking service process", service_name);

    // checks if the service has a pid

    auto pid = state.pid;
    if (pid == 0) {
      // the service has no pid, so it is not running
      Log::debug("%s: no process is assigned to the service", service_name);
      return false;
    }

    // checks if the process is running

    int wstatus = 0;

    if (!Supervisor::ReapZombieProcess(pid, &wstatus)) {
      // process is running
      Log::debug("%s: service process is running (PID=%i)", service_name, pid);
      return true;
    }

    // process is reaped, so the service is not running
    // updates the service status to broken

    state.pid = 0;
    state.status = ServiceStatus::Broken;

    if (WIFEXITED(wstatus)) {
      state.exit_code = WEXITSTATUS(wstatus);
      Log::notice(
        "%s: service process exits (PID=%i, EXITCODE=%i)",
        service_name, pid, WEXITSTATUS(wstatus)
      );
    } else {
      state.exit_code = -1;
      Log::notice(
        "%s: service process terminated by signal (PID=%i, SIGNAL=%i)",
        service_name, pid, WTERMSIG(wstatus)
      );
    }
    return false;
  }

  //////////////////////////////////////////////////////////////////////

  vector<Service*> Service::GetDependencies() {
    auto names = dependsOn();
    vector<Service*> services;

    for (auto &name : names) {
      auto service = manager->GetService(name);
      services.push_back(service);
    }

    return services;
  }
}
