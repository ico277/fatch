#include <algorithm>
#include <string>

void erase_all(std::string &str, char c) {
    str.erase(std::remove(str.begin(), str.end(), c), str.end());
}

std::string replaceAll(std::string &str, std::string from, std::string to) {
    std::size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}
