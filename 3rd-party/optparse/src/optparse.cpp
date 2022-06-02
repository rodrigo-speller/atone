
/* Original source: https://github.com/skeeto/optparse
 */

#include "optparse.h"

#define OPTPARSE_MSG_INVALID "invalid option"
#define OPTPARSE_MSG_MISSING "option requires an argument"
#define OPTPARSE_MSG_TOOMANY "option takes no arguments"

static int
optparse_error(struct optparse *options, const char *msg, const char *data)
{
    unsigned p = 0;
    const char *sep = " -- '";
    while (*msg)
        options->errmsg[p++] = *msg++;
    while (*sep)
        options->errmsg[p++] = *sep++;
    while (p < sizeof(options->errmsg) - 2 && *data)
        options->errmsg[p++] = *data++;
    options->errmsg[p++] = '\'';
    options->errmsg[p++] = '\0';
    return '?';
}

OPTPARSE_API
void
optparse_init(struct optparse *options, char **argv)
{
    options->argv = argv;
    options->optind = 1;
    options->subopt = 0;
    options->optarg = 0;
    options->errmsg[0] = '\0';
}

static int
optparse_is_dashdash(const char *arg)
{
    return arg != 0 && arg[0] == '-' && arg[1] == '-' && arg[2] == '\0';
}

static int
optparse_is_shortopt(const char *arg)
{
    return arg != 0 && arg[0] == '-' && arg[1] != '-' && arg[1] != '\0';
}

static int
optparse_is_longopt(const char *arg)
{
    return arg != 0 && arg[0] == '-' && arg[1] == '-' && arg[2] != '\0';
}

static int
optparse_longopts_end(const struct optparse_long *longopts, int i)
{
    return !longopts[i].longname && !longopts[i].shortname;
}

/* Unlike strcmp(), handles options containing "=". */
static int
optparse_longopts_match(const char *longname, const char *option)
{
    const char *a = option, *n = longname;
    if (longname == 0)
        return 0;
    for (; *a && *n && *a != '='; a++, n++)
        if (*a != *n)
            return 0;
    return *n == '\0' && (*a == '\0' || *a == '=');
}

/* Return the part after "=", or NULL. */
static char *
optparse_longopts_arg(char *option)
{
    for (; *option && *option != '='; option++);
    if (*option == '=')
        return option + 1;
    else
        return 0;
}

OPTPARSE_API
char *
optparse_arg(struct optparse *options)
{
    char *option = options->argv[options->optind];
    options->subopt = 0;
    if (option != 0)
        options->optind++;
    return option;
}

OPTPARSE_API
int
optparse(struct optparse *options,
         const struct optparse_long *longopts,
         int *longindex)
{
    int i;
    char *next;
    char *option = options->argv[options->optind];
    options->errmsg[0] = '\0';
    options->optopt = 0;
    options->optarg = 0;
    if (option == 0) {
        return -1;
    } else if (optparse_is_dashdash(option)) {
        options->optind++; /* consume "--" */
        return -1;
    } else if (!optparse_is_shortopt(option)) {
        return -1;
    }

    /* Parse as short option. */
    option += options->subopt + 1 /* skip "-" and previous subopt */;
    options->optopt = option[0];
    next = options->argv[options->optind + 1];
    const char name = *option;
    for (i = 0; !optparse_longopts_end(longopts, i); i++) {
        if (name == longopts[i].shortname) {
            if (longindex)
                *longindex = i;

            switch (longopts[i].argtype) {
            case OPTPARSE_NONE:
                if (option[1]) {
                    options->subopt++;
                } else {
                    options->subopt = 0;
                    options->optind++;
                }
                break;
            case OPTPARSE_REQUIRED:
                options->subopt = 0;
                options->optind++;
                if (option[1]) {
                    options->optarg = option + 1;
                } else if (next != 0) {
                    options->optarg = next;
                    options->optind++;
                } else {
                    char str[2] = {name, 0};
                    options->optarg = 0;
                    return optparse_error(options, OPTPARSE_MSG_MISSING, str);
                }
                break;
            case OPTPARSE_OPTIONAL:
                options->subopt = 0;
                options->optind++;
                if (option[1])
                    options->optarg = option + 1;
                else
                    options->optarg = 0;
                break;
            }

            return name;
        }
    }
    options->optind++;
    char str[2] = {name, 0};
    return optparse_error(options, OPTPARSE_MSG_INVALID, str);
}

OPTPARSE_API
int
optparse_long(struct optparse *options,
              const struct optparse_long *longopts,
              int *longindex)
{
    int i;
    char *option = options->argv[options->optind];
    options->errmsg[0] = '\0';
    options->optopt = 0;
    options->optarg = 0;
    if (option == 0) {
        return -1;
    } else if (optparse_is_dashdash(option)) {
        options->optind++; /* consume "--" */
        return -1;
    } else if (optparse_is_shortopt(option)) {
        return optparse(options, longopts, longindex);
    } else if (!optparse_is_longopt(option)) {
        return -1;
    }

    /* Parse as long option. */
    option += 2; /* skip "--" */
    options->optind++;
    for (i = 0; !optparse_longopts_end(longopts, i); i++) {
        const char *name = longopts[i].longname;
        if (optparse_longopts_match(name, option)) {
            char *arg;

            if (longindex)
                *longindex = i;

            options->optopt = longopts[i].shortname;
            arg = optparse_longopts_arg(option);
            
            if (arg != 0) {
                if (longopts[i].argtype == OPTPARSE_NONE) {
                return optparse_error(options, OPTPARSE_MSG_TOOMANY, name);
            }
                options->optarg = arg;
            } else if (longopts[i].argtype == OPTPARSE_REQUIRED) {
                options->optarg = options->argv[options->optind];
                if (options->optarg == 0)
                    return optparse_error(options, OPTPARSE_MSG_MISSING, name);
                else
                    options->optind++;
            }
            return options->optopt;
        }
    }
    return optparse_error(options, OPTPARSE_MSG_INVALID, option);
}
