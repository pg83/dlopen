#include <cstring>
#include <cstdlib>
#include <iostream>

#include "dlfcn.h"

int main() {
    auto s = dlsym(RTLD_DEFAULT, "dlerror");

    if (!s) {
        std::cerr << "no sym" << std::endl;

        abort();
    }

    auto err = ((char* (*)())(s))();

    if (strcmp(err, "not found") != 0) {
        std::cerr << "bad sym" << std::endl;

        abort();
    }
}
