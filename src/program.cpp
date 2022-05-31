#define OPTPARSE_IMPLEMENTATION

#include <iostream>
#include <unistd.h>

#include "config/AtoneOptions.h"
#include "logging/Log.h"
#include "logging/OutputLogger.h"
#include "logging/TerminalLogger.h"
#include "programs/HelpProgram.h"
#include "programs/SupervisorProgram.h"
#include "programs/VersionProgram.h"

int main(int argc, char **argv) {

    using namespace Atone;

    // GLOBAL EXCEPTION HANDLING
    try {
        // parse args

        AtoneOptions options;
        options.LoadArgs(argc, argv);

        if (!options.errorMessage.empty()) { // options error
            std::cerr << "load options failed: " << options.errorMessage << std::endl;
            return 1;
        }

        // setup

        Logger *logger;

        if (options.loggerFactory) {
            logger = options.loggerFactory(options);
        } else {
            // default logger
            if (isatty(1)) {
                logger = new TerminalLogger(options.logLevel);
            } else {
                logger = new OutputLogger(options.logLevel);
            }
        }

        Log::set(logger);

        // run

        ProgramBase *program;

        if (options.usage) {
            program = new HelpProgram();
        } else if (options.version) {
            program = new VersionProgram();
        } else {
            program = new SupervisorProgram();
        }

        return program->Run(options);
    }
    catch (char const *message) {
        Log::fatal(message);
        std::cerr << "Caught exception: " << message << std::endl;
    }
    catch (const char *message) {
        Log::fatal(message);
        std::cerr << "Caught exception: " << message << std::endl;
    }
    catch (const std::string ex) {
        auto message = ex.c_str();
        Log::fatal(message);
        std::cerr << "Caught exception: " << message << std::endl;
    }
    catch (std::exception const &ex) {
        Log::fatal(ex.what());
        std::cerr << "Caught exception: " << ex.what() << std::endl;
    }
    catch (...) {
        auto name = typeid(std::current_exception()).name();
        Log::fatal("Caught unknown exception: %s", name);
        std::cerr << "Caught unknown exception: " << name << std::endl;
    }

    exit(3);
}

