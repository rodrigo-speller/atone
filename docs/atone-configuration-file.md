# Atone configuration file

Atone uses a YAML file to configure the services that needs to be started and managed by Atone. The default configuration file is `/etc/atone.yaml`. But, you can set a custom configuration file by setting the `--config=<file>` argument when executing `atone` command.

If you're new to YAML, you can learn about it on [Learn YAML in five minutes](https://www.codeproject.com/Articles/1214409/Learn-YAML-in-five-minutes) or [YAML.org](https://yaml.org/).

## Atone settings

### `settings`

General settings section for Atone.

### `settings.workdir`

The working directory for Atone. This is the directory where the services will be executed by default. By default, the working directory is where the configuration file is located.

## Sevices

### `services`

The services section is where you define the services that will be managed by Atone.

### `services.<name>`

Each service is defined by a key that will be used to identify the service. The value of the key is a map with the service settings.

### `services.<name>.command`

The command to be executed by the service. The process started by the command will be monitored and managed by Atone.

### `services.<name>.depends_on`

A list of services that the service depends on. The service will be started after all the services listed here are started.

### `services.<name>.env`

> Not implemented yet.

### `services.<name>.restart`

The restart policy for the service to automatically restart the service when it exits.

The value must be one of:

* `no`: Do not automatically restart the container. This is the default.
* `always`: Always restart the service.
* `on-failure`: Restart the service if it exits with a non-zero exit code.
* `unless-stopped`: Restart the service unless it was explicitly stopped.

### `services.<name>.schedule`

A list of cron-like expression schedules that the service will be started. The service will be started when one of the schedules matches the current time. The syntax of the expression is the same as the [cron](https://en.wikipedia.org/wiki/Cron) command (with only five fields of the time expression).

Only one instance of the service will be started. If the service is already running, the service will not be started again.

Not setting the `schedule` value means that the service will be started when Atone starts. The same as setting the `schedule` value to `@reboot`.

### `services.<name>.workdir`

> Not implemented yet.
