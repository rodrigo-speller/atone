// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "Service.h"

#include "config/Supervisor.h"
#include "config/ServiceConfig.h"
#include "config/ServiceStatus.h"
#include "exception/AtoneException.h"
#include "logging/Log.h"
#include "utils/time.h"

namespace Atone {
  Service::Service(const ServiceConfig &config)
    : config(config) {
  }

  //////////////////////////////////////////////////////////////////////

  string Service::name() const { return config.name; }
  size_t Service::argc() const { return config.argc; }
  char **Service::argv() const { return config.argv; }
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
      pid = Supervisor::Spawn(argv);
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

    Log::info("%s: service started (PID=%i)", service_name, pid);
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
    Supervisor::ReapZombieProcess(pid, &wstatus);

    if (wstatus == 0) {
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
      Log::info(
        "%s: service process exits (PID=%i, EXITCODE=%i)",
        service_name, pid, WEXITSTATUS(wstatus)
      );
    } else {
      state.exit_code = -1;
      Log::info(
        "%s: service process terminated by signal (PID=%i, SIGNAL=%i)",
        service_name, pid, WTERMSIG(wstatus)
      );
    }
    return false;
  }
}
