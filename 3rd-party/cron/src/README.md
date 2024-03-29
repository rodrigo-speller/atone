# Vixie Cron

Atone uses the Vixie Cron code to parse cron expressions.

The original Vixie Cron code is available at [Vixie Cron on GitHub](https://github.com/vixie/cron/tree/f4311d3a1f4018b8a2927437216585d058b95681).

## Changes

The following changes have been made to the original code:

- Only the necessary files have been included in the Atone source tree.
- The included files have been stripped of unecessary code to only parse the cron expressions.
- In addition to strip unnecessary lines, only these lines of the original source code has been changed to allow parsing only cron expressions, without commands:

    - Parses only the first five fields of the cron expression:

        ```diff
        --- a/3rd-party/cron/src/cron/entry.c
        +++ b/3rd-party/cron/src/cron/entry.c


            Skip_Blanks(ch, file);
        -   if (ch == EOF || ch == '\n') {
        -       ecode = e_cmd;
        +   if (ch != EOF) {
        +       ecode = e_timespec;
                goto eof;
            }
        ```

        And:

        ```diff
        --- a/3rd-party/cron/src/cron/entry.c
        +++ b/3rd-party/cron/src/cron/entry.c

            /* DOW (days of week)
            */
            if (ch == '*')
                e->flags |= DOW_STAR;
            ch = get_list(e->dow, FIRST_DOW, LAST_DOW,
                        DowNames, ch, file);
        -   if (ch == EOF) {
        +   if (ch != EOF) {
                ecode = e_dow;
                goto eof;
            }
        ```

    - Make the project target to C17, instead of GNU17:

        ```diff
        --- a/3rd-party/cron/src/cron/externs.h
        +++ b/3rd-party/cron/src/cron/externs.h

            #include <bitstring.h>
            #include <ctype.h>
            #include <dirent.h>
            #include <pwd.h>
            #include <stdarg.h>
            #include <stdio.h>
            #include <stdlib.h>
            #include <string.h>
        +   #include <strings.h>
            #include <time.h>
        ```

## License

Copyright 1988,1990,1993,2021 by Paul Vixie ("VIXIE")<br>
Copyright (c) 2004 by Internet Systems Consortium, Inc. ("ISC")<br>
Copyright (c) 1997,2000 by Internet Software Consortium, Inc.

Vixie Cron is licensed under a permissive license. See the [LICENSE](cron/LICENSE) file for details.
