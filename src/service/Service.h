// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

namespace Atone {
  class Service;
}

#include "config/ServiceConfig.h"
#include "service/ServiceScheduler.h"
#include "service/ServicesManager.h"
#include "service/ServiceStatus.h"

namespace Atone {
  /**
   * Representa a instance of service to be managed by the
   * ServiceManager.
   * 
   * The Service class is responsible for mantaing the service's state
   * and providing the necessary methods to start, stop, restart and
   * check the service's state.
   */
  class Service {
    
  //////////////////////////////////////////////////////////////////////

  private:
    /** Represents the Service's execution state. */
    struct ServiceExecutionState {
      /** The service's status. */
      ServiceStatus status = ServiceStatus::NotStarted;
      /** The service's process ID. */
      pid_t pid = 0;
      /** The service's exit code. */
      int exit_code;
    };

    /** The service provider to resolve services dependencies. */
    const ServicesManager *manager;

    /** The service configuration */
    const ServiceConfig config;

    /** The service scheduler. */
    const ServiceScheduler *_scheduler;

    /** The service execution state. */
    ServiceExecutionState state;

    /** Private start method with callstack parameter to avoid cyclic calls. */
    void Start(vector<Service*> callstack);

    /**
     * Starts the process of the service. If the service already has a
     * running process, this method does nothing.
     */
    void StartProcess();

    /**
     * @brief Get the dependencies services of this service.
     * @return Returns the dependencies instances of this service.
     */
    vector<Service*> GetDependencies();

  //////////////////////////////////////////////////////////////////////

  public:
    /**
     * Construct a new Service instance with the given configuration.
     * @param config Service configuration.
     */
    Service(const ServicesManager *manager, const ServiceConfig &config);

    /**
     * Destroy the Service instance.
     */
    ~Service();

    ////////////////////////////////////////////////////////////////////

    /**
     * Gets the service name.
     * @return The service name.
     */
    string name() const;

    /**
     * Gets the service command arguments count.
     * @return The service command arguments count.
     */
    size_t argc() const;

    /**
     * Gets the service command arguments.
     * @return The service command arguments.
     */
    shared_ptr<char *> argv() const;

    /**
     * Gets the service dependencies.
     * @return The service dependencies.
     */
    vector<string> dependsOn() const;

    /**
     * Gets the service restart policy.
     * @return The service restart policy.
     */
    ServiceRestartPolicy restartPolicy() const;

    /**
     * Gets the service scheduler to check if the service should be started
     * based on the current time.
     * @return The service scheduler.
     */
    const ServiceScheduler *scheduler() const;

    ////////////////////////////////////////////////////////////////////

    /**
     * Get the current status of the service.
     * @returns The current status of the service.
     */
    ServiceStatus status() const;

    /**
     * Get the process ID of the service.
     * @return The process ID of the service if it is running, 0 otherwise.
     */
    pid_t pid() const;

    /**
     * Get the exit code of the last process run by this service.
     * @return The exit code of the last process run by this service.
     */
    int exitCode() const;

    ////////////////////////////////////////////////////////////////////

    /**
     * Get the service running status.
     * @return Returns true if the service is running, otherwise false.
     */
    bool isRunning() const;

    /**
     * Defines if the service can be restarted.
     */
    bool canRestart() const;

    /**
     * Starts the service, and all its dependencies if necessary.
     * If the service is already running, this method does nothing.
     */
    void Start();

    /**
     * Stops the service. If the service process is running,
     * send a SIGTERM signal. If the service is not running, this
     * method does nothing.
     * 
     * @return Returns true if the service is stopped or already is
     *         not running. Returns false if the service process is
     *         still running, after the signal is sent.
     */
    bool Stop();

    
    /**
     * Checks if the process of the service is running. If the process
     * is a zombie, it is reaped and the service status is updated.
     * 
     * @return Returns true if the process of the service is running,
     *         otherwise false.
     */
    bool CheckProcessState();
  };
}
