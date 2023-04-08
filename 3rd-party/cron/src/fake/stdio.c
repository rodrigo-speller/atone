// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

/*
 * Implementation of stdio.h for use with libcron.
 * Fake FILE struct and functions to read from a string.
*/

#include <stdio.h>

int feof(FILE *stream) {
    if (*stream->current == 0) {
        return EOF;
    }
    return 0;
}

int getc(FILE *stream) {
    if (*stream->current == 0) {
        return EOF;
    }
    return *stream->current++;
}

int ungetc(int c, FILE *stream) {
    if (stream->current == stream->begin || *(stream->current - 1) != c) {
        return EOF;
    }

    --stream->current;
    return c;
}
