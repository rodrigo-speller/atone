// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#define OPTPARSE_IMPLEMENTATION

#include <iostream>
#include <unistd.h>

#include "config/AtoneOptions.h"
#include "logging/Log.h"
#include "programs/ProgramFactory.h"

int main(int argc, char **argv) {

    using namespace Atone;

    // GLOBAL EXCEPTION HANDLING
    try {
        // parse args

        AtoneOptions options;
        options.LoadArgs(argc, argv);

        if (!options.errorMessage.empty()) { // options error
            throw std::domain_error("load options failed: " + options.errorMessage);
        }

        // setup

        Log::set(options.loggerFactory(options));

        // run

        return ProgramFactory::CreateProgram(options)->Run();
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

