#include <algorithm>
#include <string>

void erase_all(std::string &str, char c) {
    str.erase(std::remove(str.begin(), str.end(), c), str.end());
}
