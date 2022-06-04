// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "config/AtoneOptions.h"
#include "config/Supervisor.h"
#include "logging/Log.h"
#include "programs/ProgramFactory.h"

int main(int argc, char **argv) {

    using namespace Atone;

    int exitCode = EXIT_FAILURE;

    // GLOBAL EXCEPTION HANDLING
    try {
        // parse args

        const auto options = AtoneOptions::FromArgs(argc, argv);

        // setup

        Log::set(options.loggerFactory(options));
        Supervisor::Initialize();

        exitCode = ProgramFactory::CreateProgram(options)->Run();

        // shutdown
        Supervisor::Dispose();
        Log::set(nullptr);
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

    return exitCode;
}
