#define OPTPARSE_IMPLEMENTATION

#include <iostream>

#include "atone.h"
#include "logging/Log.h"

int main(int argc, char **argv) {

    using Atone::Log;

    // GLOBAL EXCEPTION HANDLING
    try {

        // call Atone's Main Program
        return Atone::main(argc, argv);

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

