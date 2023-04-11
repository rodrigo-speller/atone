// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include "atone.h"

#include "utils/CronExpression.h"
#include "utils/CronTime.h"

namespace Atone {
  class ServiceScheduler;
}

#include "service/Service.h"

namespace Atone {

    class ServiceScheduler {
        private:
            bool _on_boot;
            vector<CronExpression *> _expressions;

        public:
            ServiceScheduler(const vector<string> &config);
            ~ServiceScheduler();

            bool onBoot() const;

            bool CheckTime(const CronTime *time) const;
    };

}
