// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#define MAIN_PROGRAM

#include <cronutils.h>

#include "cron/cron.h"

typedef struct __cron_time_state_t {
    int minute;
    int hour;
    int dom;
    int month;
    int dow;
} __cron_time_state_t;

cron_expr_t *cron_expr_alloc() {
    return calloc(1, sizeof(cron_expr_t));
}

void cron_expr_free(cron_expr_t *expr_ptr) {
    if (expr_ptr == NULL) {
        return;
    }

    free_entry(expr_ptr->expr_st_ptr);
    free(expr_ptr);
}

void cron_expr_parse(char const *expr_str, cron_expr_t *expr_ptr, void (*error_func)(const char *)) {
    if (expr_ptr == NULL) {
        return;
    }

    FILE file = { expr_str, expr_str };
    entry *entry_ptr = load_entry(&file, error_func, NULL, NULL);

    if (entry_ptr == NULL) {
        return;
    }

    if (expr_ptr->expr_st_ptr != NULL) {
        free_entry(expr_ptr->expr_st_ptr);
    }

    expr_ptr->expr_st_ptr = entry_ptr;
}

int cron_expr_match(const cron_expr_t *expr_ptr, const cron_time_t *time_ptr) {
    if (expr_ptr == NULL || time_ptr == NULL) {
        return FALSE;
    }

    entry *entry_ptr = expr_ptr->expr_st_ptr;

    if (entry_ptr == NULL || time_ptr->time_st_ptr == NULL) {
        return FALSE;
    }

    __cron_time_state_t time_st = *(__cron_time_state_t*)time_ptr->time_st_ptr;

    /**
     * Begin: extracted from 'find_jobs' at cron.c.
     * https://github.com/vixie/cron/blob/f4311d3a1f4018b8a2927437216585d058b95681/cron.c#L299-L324
     * 
     * Copyright (c) 1988,1990,1993,1994,2021 by Paul Vixie ("VIXIE")
     * Copyright (c) 2004 by Internet Systems Consortium, Inc. ("ISC")
     * Copyright (c) 1997,2000 by Internet Software Consortium, Inc.
     *
     * Permission to use, copy, modify, and distribute this software for any
     * purpose with or without fee is hereby granted, provided that the above
     * copyright notice and this permission notice appear in all copies.
     *
     * THE SOFTWARE IS PROVIDED "AS IS" AND VIXIE DISCLAIMS ALL WARRANTIES
     * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
     * MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL VIXIE BE LIABLE FOR
     * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
     * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
     * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
     * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
     * 
     * Changes from original:
     * - Only the time test logic is extracted.
     * - Conditional 'doWild' and 'doNonWild' parameters are removed (always TRUE).
     * - Comments about 'dom' and 'dow' validation are changed of place to be more clear.
     */
    if (bit_test(entry_ptr->minute, time_st.minute) &&
        bit_test(entry_ptr->hour, time_st.hour) &&
        bit_test(entry_ptr->month, time_st.month) &&
        /* the dom/dow situation is odd.  '* * 1,15 * Sun' will run on the
         * first and fifteenth AND every Sunday;  '* * * * Sun' will run *only*
         * on Sundays;  '* * 1,15 * *' will run *only* the 1st and 15th.  this
         * is why we keep 'e->dow_star' and 'e->dom_star'.  yes, it's bizarre.
         * like many bizarre things, it's the standard.
         */
        ( ((entry_ptr->flags & DOM_STAR) || (entry_ptr->flags & DOW_STAR))
            ? (bit_test(entry_ptr->dow,time_st.dow) && bit_test(entry_ptr->dom,time_st.dom))
            : (bit_test(entry_ptr->dow,time_st.dow) || bit_test(entry_ptr->dom,time_st.dom))
        )
    ) {
        return TRUE;
    }
    /** End: extracted from 'find_jobs' at cron.c. */

    return FALSE;
}

int cron_expr_match_reboot(const cron_expr_t *expr_ptr) {
    if (expr_ptr == NULL) {
        return FALSE;
    }

    entry *entry_ptr = expr_ptr->expr_st_ptr;

    if (entry_ptr == NULL) {
        return FALSE;
    }

    return !!(entry_ptr->flags & WHEN_REBOOT);
}

cron_time_t *cron_time_alloc() {
    return calloc(1, sizeof(cron_time_t));
}

void cron_time_free(cron_time_t *time_ptr) {
    if (time_ptr == NULL) {
        return;
    }

    free(time_ptr->time_st_ptr);
    free(time_ptr);
}

void cron_time_set(cron_time_t *cron_time, const time_t *timer) {
    if (cron_time == NULL) {
        return;
    }

    struct tm *tm = gmtime(timer);

    if (tm == NULL) {
        return;
    }

    __cron_time_state_t *time_st = calloc(1, sizeof(__cron_time_state_t));

    if (time_st == NULL) {
        return;
    }

    /**
     * Begin: extracted from 'find_jobs' at cron.c.
     * https://github.com/vixie/cron/blob/f4311d3a1f4018b8a2927437216585d058b95681/cron.c#L287-L293
     * 
     * Copyright (c) 1988,1990,1993,1994,2021 by Paul Vixie ("VIXIE")
     * Copyright (c) 2004 by Internet Systems Consortium, Inc. ("ISC")
     * Copyright (c) 1997,2000 by Internet Software Consortium, Inc.
     *
     * Permission to use, copy, modify, and distribute this software for any
     * purpose with or without fee is hereby granted, provided that the above
     * copyright notice and this permission notice appear in all copies.
     *
     * THE SOFTWARE IS PROVIDED "AS IS" AND VIXIE DISCLAIMS ALL WARRANTIES
     * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
     * MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL VIXIE BE LIABLE FOR
     * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
     * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
     * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
     * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
     **/
    /* make 0-based values out of these so we can use them as indicies
     */
    time_st->minute = tm->tm_min -FIRST_MINUTE;
    time_st->hour = tm->tm_hour -FIRST_HOUR;
    time_st->dom = tm->tm_mday -FIRST_DOM;
    time_st->month = tm->tm_mon +1 /* 0..11 -> 1..12 */ -FIRST_MONTH;
    time_st->dow = tm->tm_wday -FIRST_DOW;
    /** End: extracted from find_jobs at cron.c **/

    if (cron_time->time_st_ptr != NULL) {
        free(cron_time->time_st_ptr);
    }

    cron_time->time_st_ptr = time_st;
}
