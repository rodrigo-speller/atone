// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#include "atone.h"

#include "AtoneException.h"

namespace Atone {

    AtoneException::AtoneException(const std::string &what) {
        _what = what;
    }

    AtoneException::AtoneException(const char *what) {
        _what = what;
    }

    const char *AtoneException::what() const noexcept {
        return _what.c_str();
    }

}