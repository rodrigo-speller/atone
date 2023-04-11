// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "CronExpression.h"

#include "CronTime.h"

namespace Atone {
    
    CronExpression::CronExpression(const CronExpression &expr) {
        _expr = cron_expr_alloc(expr._expr);
    }
    
    CronExpression::CronExpression(const char *const expr) {
        _expr = cron_expr_alloc(NULL);

        cron_expr_parse(expr, _expr, [](const char *err) {
            throw domain_error(err);
        });
    }

    CronExpression::~CronExpression() {
        cron_expr_free(_expr);
    }

    bool CronExpression::isReboot() const {
        return cron_expr_match_reboot(_expr) != 0;
    }

    bool CronExpression::Matches(const CronTime *time) const {
        return time->Matches(_expr);
    }

    bool CronExpression::Matches(const cron_time_t *time) const {
        return cron_expr_match(_expr, time) != 0;
    }

}
