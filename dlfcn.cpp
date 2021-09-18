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

            return nullptr;
        }
    };

    struct Handles: public IfaceHandle, public std::unordered_map<std::string, Handle> {
        // default handle lookup
        void* lookup(const std::string_view& s) const noexcept override {
            for (const auto& it : *this) {
                if (auto res = it.second.lookup(s); res) {
                    return res;
                }
            }

            return nullptr;
        }

        inline IfaceHandle* findHandle(const std::string_view& s) {
            if (auto it = find(std::string(s)); it != end()) {
                return &it->second;
            }

            return nullptr;
        }

        inline void registar(const char* lib, const char* symbol, void* ptr) noexcept {
            (*this)[lib][symbol] = ptr;
        }

        static inline Handles* instance() noexcept {
            static Handles h;

            return &h;
        }
    };
}

extern "C" void* stub_dlsym(void* handle, const char* symbol) {
    if (handle) {
        return ((IfaceHandle*)handle)->lookup(symbol);
    }

    return 0;
}

extern "C" void* stub_dlopen(const char* filename, int) {
    if (!filename) {
        filename = "";
    }

    return Handles::instance()->findHandle(filename);
}

extern "C" int stub_dlclose(void*) {
    return 0;
}

extern "C" char* stub_dlerror(void) {
    return (char*)"not found";
}

extern "C" void stub_dlregister(const char* lib, const char* symbol, void* ptr) {
    Handles::instance()->registar(lib, symbol, ptr);
}

DL_LIB("dl")
DL_S_2("dlopen", dlopen)
DL_S_2("dlsym", dlsym)
DL_S_2("dlclose", dlclose)
DL_S_2("dlerror", dlerror)
DL_END()
