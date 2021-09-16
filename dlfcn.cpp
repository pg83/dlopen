#include "dlfcn.h"

#include <string>
#include <unordered_map>

namespace {
    struct Handle: public std::unordered_map<std::string, void*> {
        inline void* symbol(const std::string& s) const noexcept {
            if (auto it = find(s); it != end()) {
                return it->second;
            }

            return 0;
        }
    };

    struct Handles: public std::unordered_map<std::string, Handle> {
        inline Handles() {
            (*this)[""];
        }
    };

    static inline Handles* handles() noexcept {
        static Handles h;

        return &h;
    }

    static inline Handle* findHandle(const std::string& s) {
        if (auto it = handles()->find(s); it != handles()->end()) {
            return &it->second;
        }

        return nullptr;
    }
}

extern "C" void* stub_dldefault() {
    return stub_dlopen("", 0);
}

extern "C" void* stub_dlsym(void* handle, const char* symbol) {
    return ((Handle*)handle)->symbol(symbol);
}

extern "C" void* stub_dlopen(const char* filename, int) {
    return findHandle(filename);
}

extern "C" int stub_dlclose(void*) {
    return 0;
}

extern "C" char* stub_dlerror(void) {
    return (char*)"not found";
}

extern "C" void stub_dlregister(const char* lib, const char* symbol, void* ptr) {
    (*handles())[lib][symbol] = ptr;
}

DL_LIB("dl")
DL_SYM("dlopen",  stub_dlopen)
DL_SYM("dlsym",   stub_dlsym)
DL_SYM("dlclose", stub_dlclose)
DL_SYM("dlerror", stub_dlerror)
DL_END()
