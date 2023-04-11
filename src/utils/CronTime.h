// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

namespace Atone {
    class CronTime;
}

#include "CronExpression.h"

namespace Atone {

    class CronTime {
        private:
            cron_time_t *_time;

        public:
            CronTime(const CronTime &time);
            CronTime(const time_t *const time);
            CronTime(const cron_time_t *const time);
            ~CronTime();

            void IncrementTick();
            bool Matches(const cron_expr_t *expr) const;
            bool Matches(const CronExpression *expr) const;

            bool operator == (const CronTime &other) const;
            bool operator != (const CronTime &other) const;
            bool operator < (const CronTime &other) const;
            bool operator <= (const CronTime &other) const;
            bool operator > (const CronTime &other) const;
            bool operator >= (const CronTime &other) const;
    };

}
