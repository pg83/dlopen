#include "dlfcn.h"

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
    auto s = dlsym(RTLD_DEFAULT, "dlerror");

    if (!s) {
        throw std::runtime_error("no sym");
    }

    auto err = ((char* (*)())(s))();

    if (strcmp(err, "not found") != 0) {
        throw std::runtime_error("bad sym");
    }
}
