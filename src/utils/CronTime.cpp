// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "CronTime.h"

#include "CronExpression.h"

namespace Atone {

    CronTime::CronTime(const CronTime &time) {
        _time = cron_time_alloc(time._time);
    }

    CronTime::CronTime(const time_t *const time) {
        _time = cron_time_alloc(NULL);
        cron_time_set(_time, time);
    }

    CronTime::CronTime(const cron_time_t *const time) {
        _time = cron_time_alloc(time);
    }

    CronTime::~CronTime() {
        cron_time_free(_time);
    }

    void CronTime::IncrementTick() {
        cron_time_inc(_time);
    }

    bool CronTime::Matches(const CronExpression *expr) const {
        return expr->Matches(_time);
    }

    bool CronTime::Matches(const cron_expr_t *expr) const {
        return cron_expr_match(expr, _time) != 0;
    }

    bool CronTime::operator == (const CronTime &other) const {
        return cron_time_cmp(_time, other._time) == 0;
    }

    bool CronTime::operator != (const CronTime &other) const {
        return cron_time_cmp(_time, other._time) != 0;
    }

    bool CronTime::operator > (const CronTime &other) const {
        return cron_time_cmp(_time, other._time) > 0;
    }

    bool CronTime::operator >= (const CronTime &other) const {
        return cron_time_cmp(_time, other._time) >= 0;
    }

    bool CronTime::operator < (const CronTime &other) const {
        return cron_time_cmp(_time, other._time) < 0;
    }

    bool CronTime::operator <= (const CronTime &other) const {
        return cron_time_cmp(_time, other._time) <= 0;
    }

}
