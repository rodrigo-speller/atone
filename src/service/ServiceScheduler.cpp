// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "ServiceScheduler.h"

#include "logging/Log.h"
#include "service/Service.h"
#include "utils/CronExpression.h"

namespace Atone {

    /**
     * Creates a new service scheduler, parsing the given cron expressions.
     */
    ServiceScheduler::ServiceScheduler(const vector<string> &schedules) {
        vector<CronExpression *> expressions;

        if (schedules.empty()) {
            _on_boot = true;
        } else {
            _on_boot = false;

            for (auto schedule : schedules) {
                auto expr = new CronExpression(schedule.c_str());

                if (expr->isReboot()) {
                    _on_boot = true;
                    continue;
                }

                expressions.push_back(expr);
            }
        }

        _expressions = expressions;
    }

    /**
     * Destroys the service scheduler.
     */
    ServiceScheduler::~ServiceScheduler() {
        for (auto expr : _expressions) {
            delete expr;
        }
    }

    /**
     * Checks if the scheduler is configured to reboot the system.
     */
    bool ServiceScheduler::onBoot() const {
        return _on_boot;
    }

    /**
     * Checks if the timer is compatible with the service schedule.
     * @return Returns true if the timer is compatible with the service schedule, otherwise false.
     */
    bool ServiceScheduler::CheckTime(const CronTime *time) const {
        for (auto expression : _expressions) {
            if (expression->Matches(time)) {
                return true;
            }
        }

        return false;
    }

}
