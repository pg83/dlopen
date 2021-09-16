#pragma once

#define RTLD_LAZY     1
#define RTLD_NOW      2
#define RTLD_GLOBAL   4
#define RTLD_LOCAL    8
#define RTLD_NODELETE 16
#define RTLD_NOLOAD   32
#define RTLD_DEEPBIND 64

#define RTLD_DEFAULT  (stub_dldefault())
#define RTLD_NEXT     (stub_dldefault())

#define dlsym         stub_dlsym
#define dlopen        stub_dlopen
#define dlclose       stub_dlclose
#define dlerror       stub_dlerror

#if defined(__cplusplus)
extern "C" {
#endif

void* stub_dldefault();
void* stub_dlsym(void* handle, const char* symbol);
void* stub_dlopen(const char* filename, int flags);
int   stub_dlclose(void* handle);
char* stub_dlerror(void);

#if defined(__cplusplus)
}
#endif
