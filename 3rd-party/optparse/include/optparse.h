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

#endif /* OPTPARSE_H */
