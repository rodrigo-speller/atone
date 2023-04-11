// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

namespace Atone {
    class CronExpression;
}

#include "CronTime.h"

namespace Atone {

    class CronExpression {
        private:
            cron_expr_t *_expr;

        public:
            CronExpression(const CronExpression &expr);
            CronExpression(const char *const expr);
            ~CronExpression();

            bool isReboot() const;

            bool Matches(const CronTime *time) const;
            bool Matches(const cron_time_t *time) const;
    };

}
