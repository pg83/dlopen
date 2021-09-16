#include "dlfcn.h"

#include <string>
#include <string_view>
#include <unordered_map>

namespace {
    struct IfaceHandle {
        virtual void* lookup(const std::string_view& s) const noexcept = 0;
    };

    struct Handle: public IfaceHandle, public std::unordered_map<std::string, void*> {
        void* lookup(const std::string_view& s) const noexcept override {
            if (auto it = find(std::string(s)); it != end()) {
                return it->second;
            }

            return 0;
        }
    };

    struct Handles: public std::unordered_map<std::string, Handle> {
    };

    static inline Handles* handles() noexcept {
        static Handles h;

        return &h;
    }

    struct DefaultHandle: public IfaceHandle {
        void* lookup(const std::string_view& s) const noexcept override {
            for (const auto& it : *handles()) {
                if (auto res = it.second.lookup(s); res) {
                    return res;
                }
            }

            return 0;
        }
    };

    static inline IfaceHandle* findHandle(const std::string_view& s) {
        if (s == std::string_view()) {
            static DefaultHandle dh;

            return &dh;
        }

        if (auto it = handles()->find(std::string(s)); it != handles()->end()) {
            return &it->second;
        }

        return nullptr;
    }
}

extern "C" void* stub_dlsym(void* handle, const char* symbol) {
    return ((IfaceHandle*)handle)->lookup(symbol);
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
DL_S_2("dlopen", dlopen)
DL_S_2("dlsym", dlsym)
DL_S_2("dlclose", dlclose)
DL_S_2("dlerror", dlerror)
DL_END()
