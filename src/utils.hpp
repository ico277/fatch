#include <string>
#include <iostream>

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

#define ESC "\x1b"
#define COL_RESET ESC "[0m"
#define COL_RGB(R,G,B) {std::cout << ESC << "[38;2;" << R << ";" << G << ";" << B << "m";}
#define MOVE_CUR_UP(x) {std::cout << ESC << "[" << x << "A";}
#define MOVE_CUR_DOWN(x) {std::cout << ESC << "[" << x << "B";}
#define MOVE_CUR_RIGHT(x) {std::cout << ESC << "[" << x << "C";}

void erase_all(std::string &str, char c);
std::string replaceAll(std::string &str, std::string from, std::string to);
