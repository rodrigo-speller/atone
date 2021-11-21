// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#pragma once

#include <exception>
#include <string>

namespace Atone {

    class AtoneException : public std::exception {
        private:
            std::string _what;

        public:
            explicit AtoneException(const std::string &what);

            explicit AtoneException(const char *what);

            virtual const char *what() const noexcept;
    };

}