// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

/*
 * Implementation of stdio.h for use with libcron.
 * Fake FILE struct and functions to read from a string.
*/

#define EOF (-1)

typedef struct __in_memory_string_fake_file {
    char const *current;
    const char const *begin;
} __in_memory_string_fake_file;

#define FILE __in_memory_string_fake_file

int     feof(FILE *stream),
        getc(FILE *stream),
        ungetc(int c, FILE *stream);
