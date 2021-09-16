#include "dlfcn.h"

#include <string>
#include <string_view>
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
