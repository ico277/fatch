#include <string>

#define LOG(...) { \
    std::cout << __VA_ARGS__ << "\n"; \
}
#define ERR(...) { \
    LOG("[Error] " << __VA_ARGS__); \
}
#define WARN(...) { \
    LOG("[Warning] " << __VA_ARGS__); \
}
#ifdef DEBUG
#define DBG(...) { \
    LOG("[DEBUG] " << __VA_ARGS__); \
}
#else
#define DBG(...) {}
#endif

void erase_all(std::string &str, char c);
