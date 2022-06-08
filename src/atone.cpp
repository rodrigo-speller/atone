// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "config/AtoneOptions.h"
#include "logging/Log.h"
#include "programs/ProgramFactory.h"
#include "system/Supervisor.h"

int main(int argc, char **argv) {

    using namespace Atone;

    int exitcode = EXIT_FAILURE;

    // GLOBAL EXCEPTION HANDLING
    try {
        const auto options = AtoneOptions::FromArgs(argc, argv);
        const auto program = ProgramFactory::CreateProgram(options);

        exitcode = program->Run();
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

    return exitcode;
}
