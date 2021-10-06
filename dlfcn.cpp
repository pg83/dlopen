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

            return this;
        }

        inline void registar(const char* lib, const char* symbol, void* ptr) noexcept {
            (*this)[lib][symbol] = ptr;
        }

        static inline Handles* instance() noexcept {
            static Handles h;

            return &h;
        }
    };

    static thread_local const char* DL_ERROR = nullptr;

    static inline void setLastError(const char* err) {
        DL_ERROR = err;
    }

    static auto lastError() noexcept {
        auto ret = DL_ERROR;

        DL_ERROR = nullptr;

        return ret;
    }
}

extern "C" void* stub_dlsym(void* handle, const char* symbol) {
    lastError();

    if (handle) {
        if (auto ret = ((IfaceHandle*)handle)->lookup(symbol); ret) {
            return ret;
        }
    }

    setLastError("symbol not found");

    return 0;
}

extern "C" void* stub_dlopen(const char* filename, int) {
    lastError();

    if (!filename) {
        filename = "";
    }

    if (auto ret = Handles::instance()->findHandle(filename); ret) {
        return ret;
    }

    setLastError("library not found");

    return 0;
}

extern "C" int stub_dlclose(void*) {
    lastError();

    return 0;
}

extern "C" char* stub_dlerror(void) {
    return (char*)lastError();
}

extern "C" void stub_dlregister(const char* lib, const char* symbol, void* ptr) {
    Handles::instance()->registar(lib, symbol, ptr);
}

extern "C" int stub_dladdr(void* /*addr*/, Dl_info* /*info*/) {
    return 0;
}

DL_LIB("dl")
DL_S_2("dlopen", stub_dlopen)
DL_S_2("dlsym", stub_dlsym)
DL_S_2("dlclose", stub_dlclose)
DL_S_2("dlerror", stub_dlerror)
DL_S_2("dladdr", stub_dladdr)
DL_END()
