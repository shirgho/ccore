#ifdef __linux__
#define LINUX
#elif _WIN32
#define WIN32
#else
#error "OS not supported!"
#endif
