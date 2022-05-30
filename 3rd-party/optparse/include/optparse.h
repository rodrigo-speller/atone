/* Optparse --- portable, reentrant, embeddable, getopt-like option parser
 *
 * This is free and unencumbered software released into the public domain.
 *
 * To get the implementation, define OPTPARSE_IMPLEMENTATION.
 * Optionally define OPTPARSE_API to control the API's visibility
 * and/or linkage (static, __attribute__, __declspec).
 *
 * The POSIX getopt() option parser has three fatal flaws. These flaws
 * are solved by Optparse.
 *
 * 1) Parser state is stored entirely in global variables, some of
 * which are static and inaccessible. This means only one thread can
 * use getopt(). It also means it's not possible to recursively parse
 * nested sub-arguments while in the middle of argument parsing.
 * Optparse fixes this by storing all state on a local struct.
 *
 * 2) The POSIX standard provides no way to properly reset the parser.
 * This means for portable code that getopt() is only good for one
 * run, over one argv with one option string. It also means subcommand
 * options cannot be processed with getopt(). Most implementations
 * provide a method to reset the parser, but it's not portable.
 * Optparse provides an optparse_arg() function for stepping over
 * subcommands and continuing parsing of options with another option
 * string. The Optparse struct itself can be passed around to
 * subcommand handlers for additional subcommand option parsing. A
 * full reset can be achieved by with an additional optparse_init().
 *
 * 3) Error messages are printed to stderr. This can be disabled with
 * opterr, but the messages themselves are still inaccessible.
 * Optparse solves this by writing an error message in its errmsg
 * field. The downside to Optparse is that this error message will
 * always be in English rather than the current locale.
 *
 * Optparse should be familiar with anyone accustomed to getopt(), and
 * it could be a nearly drop-in replacement. The fields have the same
 * names as the getopt() global variables (optarg, optind, optopt).
 *
 * Optparse also supports GNU-style long options with optparse_long().
 * The interface is slightly different and simpler than getopt_long().
 * 
 * Original source: https://github.com/skeeto/optparse
 */
#ifndef OPTPARSE_H
#define OPTPARSE_H

#ifndef OPTPARSE_API
#  define OPTPARSE_API
#endif

struct optparse {
    /** Original parsing argurments. */
    char **argv;
    /** Current argument index. */
    int optind;
    /** Current option shortname. */
    int optopt;
    /** Current option argument. */
    char *optarg;
    /* Aggregated short option index */
    int subopt;
    /** Current error message. */
    char errmsg[64];
};

enum optparse_argtype {
    OPTPARSE_NONE,
    OPTPARSE_REQUIRED,
    OPTPARSE_OPTIONAL
};

struct optparse_long {
    /** Long option name. */
    const char *longname;
    /** Short option name. */
    int shortname;
    /** Option argument type. */
    enum optparse_argtype argtype;
};

/**
 * Initializes the parser state.
 */
OPTPARSE_API
void optparse_init(struct optparse *options, char **argv);

/**
 * Read the next option in the argv array.
 * @return the next option character, -1 for done, or '?' for error
 */
OPTPARSE_API
int optparse(
             struct optparse *options,
             const struct optparse_long *longopts,
             int *longindex);

/**
 * Handles GNU-style long options in addition to getopt() options.
 * This works a lot like GNU's getopt_long(). The last option in
 * longopts must be all zeros, marking the end of the array. The
 * longindex argument may be NULL.
 */
OPTPARSE_API
int optparse_long(struct optparse *options,
                  const struct optparse_long *longopts,
                  int *longindex);

/**
 * Used for stepping over non-option arguments.
 * @return the next non-option argument, or NULL for no more arguments
 *
 * Argument parsing can continue with optparse() after using this
 * function. That would be used to parse the options for the
 * subcommand returned by optparse_arg(). This function allows you to
 * ignore the value of optind.
 */
OPTPARSE_API
char *optparse_arg(struct optparse *options);

/* Implementation */
#ifdef OPTPARSE_IMPLEMENTATION

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

#endif /* OPTPARSE_IMPLEMENTATION */
#endif /* OPTPARSE_H */
