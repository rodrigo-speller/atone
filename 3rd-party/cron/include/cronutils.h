// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#ifndef CRONUTILS_H
#define CRONUTILS_H

#ifndef CRON_API_EXPORT

#ifdef __cplusplus
#define CRON_API_EXPORT extern "C"
#else
#define CRON_API_EXPORT
#endif

#endif

#include <time.h>

/**
 * Cron expression state.
 * 
 * This struct is opaque and should not be accessed directly.
 */
typedef struct cron_expr_t {
    void *expr_st_ptr;
} cron_expr_t;

/**
 * Cron time state.
 * 
 * This struct is opaque and should not be accessed directly.
 */
typedef struct cron_time_t {
    void *time_st_ptr;
} cron_time_t;

/**
 * Allocate a new cron expression state.
 */
CRON_API_EXPORT
cron_expr_t *cron_expr_alloc(const cron_expr_t *const clone_expr_ptr);

/**
 * Free a cron expression state.
 */
CRON_API_EXPORT
void cron_expr_free(cron_expr_t *expr_ptr);

/**
 * Parse a cron expression string into a cron expression state. If an
 * error occurs, the error_func is called with the error cause message.
 */
CRON_API_EXPORT
void cron_expr_parse(
                     char const *expr_str,
                     cron_expr_t *expr_ptr,
                     void (*error_func)(const char *));

/**
 * Match a cron expression state against a cron time state to determine
 * if the time matches the expression.
 */
CRON_API_EXPORT
int cron_expr_match(
                    const cron_expr_t *expr_ptr,
                    const cron_time_t *time_ptr);

/**
 * Match a cron expression state to determine if that expression matches
 * the "@reboot" expression. This is a special expression that matches
 * only once, when the cron is started.
 */
CRON_API_EXPORT
int cron_expr_match_reboot(const cron_expr_t *expr_ptr);

/**
 * Allocate a new cron time state. If 'clone_time_ptr' is not NULL, the
 * new cron time state will be initialized as a clone of the given cron
 * time state.
 */
CRON_API_EXPORT
cron_time_t *cron_time_alloc(const cron_time_t *const clone_time_ptr);

/**
 * Free a cron time state.
 */
CRON_API_EXPORT
void cron_time_free(cron_time_t *time_ptr);

/**
 * Set the time of a cron time state.
 */
CRON_API_EXPORT
void cron_time_set(cron_time_t *time_ptr, const time_t *timer);

/**
 * Compare two cron time states.
 */
CRON_API_EXPORT
int cron_time_cmp(const cron_time_t *const a, const cron_time_t *const b);

/**
 * Increment a cron time state by one minute.
 */
CRON_API_EXPORT
void cron_time_inc(cron_time_t *const cron_time);

#endif
